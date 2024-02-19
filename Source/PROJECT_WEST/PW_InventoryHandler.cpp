// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_InventoryHandler.h"

#include "DebugMacros.h"
#include "PW_Character.h"
#include "PW_InventorySlot.h"
#include "PW_ItemObject.h"
#include "PW_Utilities.h"

UPW_InventoryHandler::UPW_InventoryHandler()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPW_InventoryHandler::BeginPlay()
{
	Super::BeginPlay();
	
	AssignInputActions();
	GetOwnerCharacter();
	CreateInventoryConfiguration();

	//DEBUG THINGS >>
	FTimerHandle spawnItemTimer;
	GetWorld()->GetTimerManager().SetTimer(spawnItemTimer, this,
		&UPW_InventoryHandler::SpawnItem, 2.0f, false);
	//DEBUG THINGS <<
}

void UPW_InventoryHandler::SpawnItem()
{
	APW_ItemObject* newItem = GetWorld()->SpawnActor<APW_ItemObject>(_spawnItemClass,
		FVector(0, 0, 0), FRotator(0, 0, 0));
	
	TryCollectItem(newItem);
}

void UPW_InventoryHandler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPW_InventoryHandler::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UPW_InventoryHandler::UseCurrentItem()
{
	const UPW_InventorySlot* currentSlot = GetCurrentSlot();

	if (currentSlot == nullptr)
		{ PW_Utilities::Log("CURRENT SLOT IS NULL!"); return; }

	APW_ItemObject* currentItem = currentSlot->GetCurrentItem();
	
	if (currentItem == nullptr)
		{ PW_Utilities::Log("CURRENT ITEM IS NULL!"); return; }

	currentItem->Use();
}

bool UPW_InventoryHandler::TryGetSlot(int slotIndex, UPW_InventorySlot*& outSlot)
{
	const bool isIndexValid = slotIndex >= 0 && slotIndex < _slotConfiguration.Num();
	
	if (!isIndexValid)
		{ PW_Utilities::Log("INVALID SLOT INDEX!"); return false; }

	outSlot = GetSlot(slotIndex);
	return true;
}

void UPW_InventoryHandler::ChangeSlot(const UPW_InventorySlot* updatedSlot)
{
	if (updatedSlot == nullptr)
		{ PW_Utilities::Log("UPDATED SLOT IS NULL!"); return; }
	
	const UPW_InventorySlot* currentSlot = GetCurrentSlot();

	if (currentSlot == updatedSlot)
		{ PW_Utilities::Log("CURRENT SLOT IS THE SAME AS UPDATED SLOT!"); return; }

	APW_ItemObject* currentItem = currentSlot->GetCurrentItem();
	
	if (currentItem != nullptr)
		HideItem(currentItem);

	APW_ItemObject* updatedItem = updatedSlot->GetCurrentItem();
	
	if (updatedItem != nullptr)
		ShowItem(updatedItem);
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
	collectedItem->SetItemState(EItemObjectState::EHeld);
	collectedItem->SetOwner(_ownerCharacter);
	return true;
}

bool UPW_InventoryHandler::TryDropItem(UPW_InventorySlot* currentSlot)
{
	if (currentSlot == nullptr)
		{ PW_Utilities::Log("CURRENT SLOT IS NULL!"); return false; }
	
	APW_ItemObject* slotItem = currentSlot->GetCurrentItem();
	
	if (slotItem == nullptr)
		{ PW_Utilities::Log("SLOT IS EMPTY!"); return false; }

	currentSlot->RemoveItem();
	slotItem->UpdateItemState(EItemObjectState::EDropped);
	slotItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	slotItem->SetItemState(EItemObjectState::EDropped);
	slotItem->SetOwner(nullptr);
	return true;
}

void UPW_InventoryHandler::DropCurrentItem()
{
	UPW_InventorySlot* currentSlot = GetCurrentSlot();

	if (currentSlot == nullptr)
		{ PW_Utilities::Log("CURRENT SLOT IS NULL!"); return; }
	
	const bool successfulDrop = TryDropItem(currentSlot);
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

void UPW_InventoryHandler::ShowItem(APW_ItemObject* selectedItem)
{
	if (selectedItem == nullptr)
		{ PW_Utilities::Log("SELECTED ITEM IS NULL!"); return; }
	
	selectedItem->SetVisibility(true);
}

void UPW_InventoryHandler::HideItem(APW_ItemObject* selectedItem)
{
	if (selectedItem == nullptr)
		{ PW_Utilities::Log("SELECTED ITEM IS NULL!"); return; }
	
	selectedItem->SetVisibility(false);
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

void UPW_InventoryHandler::CreateInventoryConfiguration()
{
	for (const FInventorySlot inventorySlotConfiguration : _slotConfiguration)
	{
		UPW_InventorySlot* createdSlot = NewObject<UPW_InventorySlot>();
		createdSlot->SetSlotType(inventorySlotConfiguration.SlotType);
		_inventorySlots.Add(createdSlot);
	}
}

void UPW_InventoryHandler::AssignInputActions()
{
	
}

