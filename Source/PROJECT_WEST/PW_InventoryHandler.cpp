// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_InventoryHandler.h"

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
}

void UPW_InventoryHandler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPW_InventoryHandler::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool UPW_InventoryHandler::TryCollectItem(APW_ItemObject* collectedItem)
{
	if (collectedItem == nullptr)
		{ PW_Utilities::Log("COLLECTED ITEM IS NULL!"); return false; }

	const EItemType itemType = collectedItem->GetItemType();
	UPW_InventorySlot* availableSlot = GetAvailableSlot(itemType);

	if (availableSlot == nullptr)
		{ PW_Utilities::Log("NO AVAILABLE SLOT!"); return false; }

	availableSlot->AppendItem(collectedItem);
	
	USceneComponent* itemPosition  = _ownerCharacter->GetItemHolder();
	collectedItem->AttachToComponent(itemPosition, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	collectedItem->SetItemState(EItemObjectState::EHeld);
	collectedItem->SetOwner(_ownerCharacter);
	return true;
}

bool UPW_InventoryHandler::TryDropItem(APW_ItemObject* droppedItem)
{
	if (droppedItem == nullptr)
		{ PW_Utilities::Log("DROPPED ITEM IS NULL!"); return false; }

	UPW_InventorySlot* currentSlot = GetCurrentSlot();
	
	if (currentSlot == nullptr)
		{ PW_Utilities::Log("NO CURRENT SLOT!"); return false; }

	currentSlot->RemoveItem(droppedItem);
	droppedItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	droppedItem->SetItemState(EItemObjectState::EDropped);
	droppedItem->SetOwner(nullptr);
	return true;
}

UPW_InventorySlot* UPW_InventoryHandler::GetAvailableSlot(EItemType itemType)
{
	for (UPW_InventorySlot* inventorySlot : _inventorySlots)
	{
		if (inventorySlot->GetSlotType() != itemType)
			return nullptr;

		if (!inventorySlot->IsSlotAvailable())
			return nullptr;

		return inventorySlot;
	}
	return nullptr;
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

void UPW_InventoryHandler::AssignInputActions()
{
	
}

