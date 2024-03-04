// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_InventoryHandler.h"

#include "PW_Character.h"
#include "PW_InventorySlot.h"
#include "PW_ItemObject.h"
#include "PW_Utilities.h"
#include "Net/UnrealNetwork.h"

UPW_InventoryHandler::UPW_InventoryHandler(): _ownerCharacter(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPW_InventoryHandler::BeginPlay()
{
	Super::BeginPlay();
	
	GetOwnerCharacter();
	CreateInventoryConfiguration();

	if (_ownerCharacter == nullptr)
		{ PW_Utilities::Log("OWNER CHARACTER NOT FOUND!"); return; }

	if (_ownerCharacter->IsLocallyControlled())
		AssignInputActions();

	const UPW_InventorySlot* currentSlot = GetCurrentSlot();
	ChangeSlot(currentSlot, true);
}

void UPW_InventoryHandler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPW_InventoryHandler::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool UPW_InventoryHandler::TryGetSlot(int slotIndex, UPW_InventorySlot*& outSlot)
{
	const bool isIndexValid = slotIndex >= 0 && slotIndex < _slotConfiguration.Num();
	
	if (!isIndexValid)
		{ PW_Utilities::Log("INVALID SLOT INDEX!"); return false; }

	outSlot = GetSlot(slotIndex);
	return true;
}

void UPW_InventoryHandler::ChangeSlot(const UPW_InventorySlot* targetedSlot, bool forceChangeSlot)
{
	if (targetedSlot == nullptr)
		{ PW_Utilities::Log("UPDATED SLOT IS NULL!"); return; }
	
	const UPW_InventorySlot* currentSlot = GetCurrentSlot();

	if (currentSlot == targetedSlot && !forceChangeSlot)
		{ PW_Utilities::Log("CURRENT SLOT IS THE SAME AS UPDATED SLOT!"); return; }

	APW_ItemObject* currentItem = currentSlot->GetItem();
	
	if (currentItem != nullptr)
		currentItem->DisableItem(_ownerCharacter);

	APW_ItemObject* updatedItem = targetedSlot->GetItem();
	
	if (updatedItem != nullptr)
		updatedItem->EnableItem(_ownerCharacter);
}

#pragma region CollectItem
void UPW_InventoryHandler::CollectItem(APW_ItemObject* collectedItem)
{
	if (collectedItem == nullptr)
		{ PW_Utilities::Log("COLLECTED ITEM IS NULL!"); return; }

	const EItemType itemType = collectedItem->GetItemType();
	UPW_InventorySlot* availableSlot = nullptr;
	const bool foundSlot = TryGetAvailableSlot(itemType, availableSlot);
	
	if (!foundSlot)
		{ PW_Utilities::Log("NO AVAILABLE SLOT!"); return; }

	availableSlot->SetItem(collectedItem);
	
	_ownerCharacter->HasAuthority() ? LocalCollectItem(collectedItem) : ServerCollectItem(collectedItem);

	FTimerHandle itemTimer;
	FTimerDelegate itemDelegate;

	itemDelegate.BindLambda([collectedItem, this]()
	{
		const UPW_InventorySlot* currentSlot = GetCurrentSlot();
		ChangeSlot(currentSlot, true);
	});

	GetWorld()->GetTimerManager().SetTimer(itemTimer, itemDelegate, 0.05f, false);
}

void UPW_InventoryHandler::LocalCollectItem(APW_ItemObject* collectedItem)
{
	if (collectedItem == nullptr)
		{ PW_Utilities::Log("COLLECTED ITEM IS NULL!"); return; }
	
	USceneComponent* itemPosition  = _ownerCharacter->GetItemHolder();

	if (itemPosition == nullptr)
		{ PW_Utilities::Log("ITEM POSITION IS NULL!"); return; }
	
	collectedItem->SetOwner(_ownerCharacter);
	collectedItem->UpdateItemState(EItemObjectState::EHeld);
	collectedItem->AttachToComponent(itemPosition, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	collectedItem->SetVisibility(false);
}

void UPW_InventoryHandler::ServerCollectItem_Implementation(APW_ItemObject* collectedItem)
{
	if (_ownerCharacter->HasAuthority())
		LocalCollectItem(collectedItem);
}
#pragma endregion CollectItem

bool UPW_InventoryHandler::TryGetAvailableSlot(EItemType itemType, UPW_InventorySlot*& outSlot)
{
	for (UPW_InventorySlot* inventorySlot : _inventorySlots)
	{
		if (inventorySlot->GetSlotType() != itemType)
			continue;

		if (!inventorySlot->IsSlotAvailable())
			continue;

		outSlot = inventorySlot;
		return true;
	}
	return false; 
}

void UPW_InventoryHandler::CreateInventoryConfiguration()
{
	for (const FInventorySlot inventorySlotConfiguration : _slotConfiguration)
	{
		UPW_InventorySlot* createdSlot = NewObject<UPW_InventorySlot>();
		createdSlot->SetSlotType(inventorySlotConfiguration.SlotType);
		_inventorySlots.Add(createdSlot);
	}
}

void UPW_InventoryHandler::CycleNextSlot()
{
	const int targetedSlotIndex = _currentSlotIndex + 1;
	constexpr int overflowSlotIndex = 0;
	const bool isIndexValid = _inventorySlots.IsValidIndex(targetedSlotIndex);

	if (isIndexValid)
	{
		const UPW_InventorySlot* targetedSlot = GetSlot(targetedSlotIndex);
		ChangeSlot(targetedSlot);
		_currentSlotIndex = targetedSlotIndex;
	}
	else
	{
		const UPW_InventorySlot* errorSlot = GetSlot(overflowSlotIndex);
		ChangeSlot(errorSlot);
		_currentSlotIndex = overflowSlotIndex;
	}
}

void UPW_InventoryHandler::CyclePreviousSlot()
{
	const int targetedSlotIndex = _currentSlotIndex - 1;
	const int overflowSlotIndex = _inventorySlots.Num() - 1;
	const bool isIndexValid = _inventorySlots.IsValidIndex(targetedSlotIndex);

	if (isIndexValid)
	{
		const UPW_InventorySlot* targetedSlot = GetSlot(targetedSlotIndex);
		ChangeSlot(targetedSlot);
		_currentSlotIndex = targetedSlotIndex;
	}
	else
	{
		const UPW_InventorySlot* errorSlot = GetSlot(overflowSlotIndex);
		ChangeSlot(errorSlot);
		_currentSlotIndex = overflowSlotIndex;
	}
}

void UPW_InventoryHandler::CycleUp()
{
	if (_inventorySlots.IsEmpty())
		{ PW_Utilities::Log("INVENTORY SLOTS ARE EMPTY, CANNOT CYCLE UP!"); return; }
	
	CycleNextSlot();
}

void UPW_InventoryHandler::CycleDown()
{
	if (_inventorySlots.IsEmpty())
		{ PW_Utilities::Log("INVENTORY SLOTS ARE EMPTY, CANNOT CYCLE DOWN!"); return; }
	
	CyclePreviousSlot();
}

void UPW_InventoryHandler::ToSlot(int slotIndex)
{
	UPW_InventorySlot* targetedSlot = nullptr;
	const bool foundSlot = TryGetSlot(slotIndex, targetedSlot);

	if (!foundSlot)
		{ PW_Utilities::Log("TARGETED SLOT NOT FOUND!"); return; }

	ChangeSlot(targetedSlot);
	_currentSlotIndex = slotIndex;
}

void UPW_InventoryHandler::DropCurrentItem()
{
	DropItem(_currentSlotIndex);
}

void UPW_InventoryHandler::DropAll()
{
	for (int i = 0; i < _inventorySlots.Num(); i++)
		DropItem(i);
}

void UPW_InventoryHandler::AssignInputActions()
{
	if (_ownerCharacter == nullptr)
		{ PW_Utilities::Log("OWNER CHARACTER NOT FOUND!"); return; }
	
	_ownerCharacter->OnCycleItemButtonPressed.AddDynamic(this, &UPW_InventoryHandler::CycleNextSlot);
	_ownerCharacter->OnMouseDownPressed.AddDynamic(this, &UPW_InventoryHandler::CycleDown);
	_ownerCharacter->OnMouseUpPressed.AddDynamic(this, &UPW_InventoryHandler::CycleUp);
	_ownerCharacter->OnDropButtonPressed.AddDynamic(this, &UPW_InventoryHandler::DropCurrentItem);
	_ownerCharacter->OnSlotPressed.AddDynamic(this, &UPW_InventoryHandler::ToSlot);
}

void UPW_InventoryHandler::GetOwnerCharacter()
{
	AActor* ownerActor = GetOwner();
	
	if (ownerActor == nullptr)
		{ PW_Utilities::Log("OWNER ACTOR NOT FOUND!"); return; }

	_ownerCharacter = Cast<APW_Character>(ownerActor);
	
	if (_ownerCharacter == nullptr)
		{ PW_Utilities::Log("OWNER CHARACTER NOT FOUND!"); }
}

#pragma region DropItem
void UPW_InventoryHandler::DropItem(int slotIndex)
{
	UPW_InventorySlot* inventorySlot = nullptr;
	const bool foundSlot = TryGetSlot(slotIndex, inventorySlot);

	if (!foundSlot)
		{ PW_Utilities::Log("[LOCAL] SLOT NOT FOUND!"); return; }

	if (inventorySlot == nullptr)
		{ PW_Utilities::Log("[LOCAL] CURRENT SLOT IS NULL!"); return; }

	APW_ItemObject* slotItem = inventorySlot->GetItem();

	if (slotItem == nullptr)
		{ PW_Utilities::Log("SLOT IS EMPTY!"); return; }

	inventorySlot->RemoveItem();
	slotItem->RemoveActionBindings(_ownerCharacter);
	slotItem->SetVisibility(true);
	
	_ownerCharacter->HasAuthority() ? LocalDropItem(slotItem) : ServerDropItem(slotItem);
}

void UPW_InventoryHandler::LocalDropItem(APW_ItemObject* slotItem)
{
	if (slotItem == nullptr)
		{ PW_Utilities::Log("[LOCAL] SLOT ITEM IS NULL!"); return; }
	
	slotItem->UpdateItemState(EItemObjectState::EDropped);
	slotItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	slotItem->SetOwner(nullptr);
}

void UPW_InventoryHandler::ServerDropItem_Implementation(APW_ItemObject* slotItem)
{
	if (_ownerCharacter->HasAuthority())
		LocalDropItem(slotItem);
}

#pragma endregion DropItem
