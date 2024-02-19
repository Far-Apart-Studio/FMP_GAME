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

	//Debug Testing >>

	FTimerHandle spawnItemTimer;
	GetWorld()->GetTimerManager().SetTimer(spawnItemTimer, this,
		&UPW_InventoryHandler::SpawnItem, 2.0f, false);

	//Debug Testing <<
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

bool UPW_InventoryHandler::TryGetSlot(int slotIndex, FInventorySlot& outSlot)
{
	if (slotIndex < 0 || slotIndex >= _currentSlots.Num())
		{ PW_Utilities::Log("INVALID SLOT INDEX!"); return false; }

	outSlot = GetSlot(slotIndex);
	return true;
}

void UPW_InventoryHandler::ChangeSlot(const FInventorySlot& updatedSlot)
{
	const FInventorySlot currentSlot = GetCurrentSlot();
	APW_ItemObject* currentItem = currentSlot.GetCurrentItem();

	if (currentItem != nullptr)
		HideItem(currentItem);

	APW_ItemObject* updatedItem = updatedSlot.GetCurrentItem();
	
	if (updatedItem != nullptr)
		ShowItem(updatedItem);
}

bool UPW_InventoryHandler::TryCollectItem(APW_ItemObject* collectedItem)
{
	if (collectedItem == nullptr)
		{ PW_Utilities::Log("COLLECTED ITEM IS NULL!"); return false; }

	const EItemType itemType = collectedItem->GetItemType();
	
	FInventorySlot availableSlot = TryGetAvailableSlot(itemType);
	availableSlot.SetItem(collectedItem);
	
	USceneComponent* itemPosition  = _ownerCharacter->GetItemHolder();
	collectedItem->AttachToComponent(itemPosition, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	collectedItem->SetItemState(EItemObjectState::EHeld);
	collectedItem->SetOwner(_ownerCharacter);
	return true;
}

bool UPW_InventoryHandler::TryDropItem(FInventorySlot& currentSlot)
{
	APW_ItemObject* slotItem = currentSlot.GetCurrentItem();
	
	if (slotItem == nullptr)
		{ PW_Utilities::Log("SLOT IS EMPTY!"); return false; }

	currentSlot.RemoveItem();
	slotItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	slotItem->SetItemState(EItemObjectState::EDropped);
	slotItem->SetOwner(nullptr);
	return true;
}

void UPW_InventoryHandler::DropCurrentItem()
{
	FInventorySlot currentSlot = GetCurrentSlot();
	const bool successfulDrop = TryDropItem(currentSlot);
}

FInventorySlot UPW_InventoryHandler::TryGetAvailableSlot(EItemType itemType)
{
	for (FInventorySlot inventorySlot : _currentSlots)
	{
		if (inventorySlot.GetSlotType() != itemType)
			continue;

		if (!inventorySlot.IsSlotAvailable())
			continue;

		return inventorySlot;
	}

	return _currentSlots[0];
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

