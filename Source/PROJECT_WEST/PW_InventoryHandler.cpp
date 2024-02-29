// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_InventoryHandler.h"

#include "PW_Character.h"
#include "PW_InventorySlot.h"
#include "PW_ItemObject.h"
#include "PW_Utilities.h"

UPW_InventoryHandler::UPW_InventoryHandler(): _ownerCharacter(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPW_InventoryHandler::BeginPlay()
{
	Super::BeginPlay();
	
	GetOwnerCharacter();
	AssignInputActions();
	CreateInventoryConfiguration();

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

void UPW_InventoryHandler::ChangeSlot(const UPW_InventorySlot* updatedSlot, bool forceChangeSlot)
{
	if (updatedSlot == nullptr)
		{ PW_Utilities::Log("UPDATED SLOT IS NULL!"); return; }
	
	const UPW_InventorySlot* currentSlot = GetCurrentSlot();

	if (currentSlot == updatedSlot && !forceChangeSlot)
		{ PW_Utilities::Log("CURRENT SLOT IS THE SAME AS UPDATED SLOT!"); return; }

	APW_ItemObject* currentItem = currentSlot->GetItem();
	
	if (currentItem != nullptr)
		DisableItem(currentItem);

	APW_ItemObject* updatedItem = updatedSlot->GetItem();
	
	if (updatedItem != nullptr)
		EnableItem(updatedItem);
}

bool UPW_InventoryHandler::TryCollectItem(APW_ItemObject* collectedItem)
{
	if (collectedItem == nullptr)
		{ PW_Utilities::Log("COLLECTED ITEM IS NULL!"); return false; }

	const EItemType itemType = collectedItem->GetItemType();

	UPW_InventorySlot* availableSlot = nullptr;
	const bool foundSlot = TryGetAvailableSlot(itemType, availableSlot);

	if (!foundSlot)
		{ PW_Utilities::Log("NO AVAILABLE SLOT!"); return false; }
	
	availableSlot->SetItem(collectedItem);
	
	USceneComponent* itemPosition  = _ownerCharacter->GetItemHolder();
	collectedItem->UpdateItemState(EItemObjectState::EHeld);
	collectedItem->AttachToComponent(itemPosition, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	collectedItem->SetOwner(_ownerCharacter);
	collectedItem->SetVisibility(false);

	const UPW_InventorySlot* currentSlot = GetCurrentSlot();
	ChangeSlot(currentSlot, true);
	
	return true;
}

void UPW_InventoryHandler::DropItem(UPW_InventorySlot* inventorySlot)
{
	if (inventorySlot == nullptr)
		{ PW_Utilities::Log("CURRENT SLOT IS NULL!"); return; }
	
	APW_ItemObject* slotItem = inventorySlot->GetItem();
	
	if (slotItem == nullptr)
		{ PW_Utilities::Log("SLOT IS EMPTY!"); return; }

	inventorySlot->RemoveItem();
	slotItem->UpdateItemState(EItemObjectState::EDropped);
	slotItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	slotItem->SetOwner(nullptr);
}

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

void UPW_InventoryHandler::EnableItem(APW_ItemObject* inventoryItem)
{
	if (inventoryItem == nullptr)
		{ PW_Utilities::Log("SELECTED ITEM IS NULL!"); return; }
	
	inventoryItem->SetVisibility(true);
	inventoryItem->ApplyBindingActions(_ownerCharacter);
}

void UPW_InventoryHandler::DisableItem(APW_ItemObject* inventoryItem)
{
	if (inventoryItem == nullptr)
		{ PW_Utilities::Log("SELECTED ITEM IS NULL!"); return; }
	
	inventoryItem->SetVisibility(false);
	inventoryItem->RemoveBindingActions(_ownerCharacter);
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
	UPW_InventorySlot* currentSlot = GetCurrentSlot();

	if (currentSlot == nullptr)
		{ PW_Utilities::Log("CURRENT SLOT IS NULL!"); return; }

	APW_ItemObject* slotItem = currentSlot->GetItem();

	if (slotItem == nullptr)
		{ PW_Utilities::Log("SLOT ITEM IS NULL!"); return; }
	
	DropItem(currentSlot);
	slotItem->RemoveBindingActions(_ownerCharacter);
}

void UPW_InventoryHandler::DropAll()
{
	for (UPW_InventorySlot* inventorySlot : _inventorySlots)
	{
		APW_ItemObject* slotItem = inventorySlot->GetItem();
		
		if (slotItem != nullptr)
			slotItem->SetVisibility(true);
		
		DropItem(inventorySlot);
	}
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

