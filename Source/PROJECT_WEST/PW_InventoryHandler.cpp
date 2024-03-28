// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_InventoryHandler.h"

#include "DebugMacros.h"
#include "PW_Character.h"
#include "PW_InventorySlot.h"
#include "PW_ItemObject.h"
#include "PW_Utilities.h"
#include "Net/UnrealNetwork.h"

UPW_InventoryHandler::UPW_InventoryHandler(): _ownerCharacter(nullptr), _ItemDataTable(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPW_InventoryHandler::BeginPlay()
{
	Super::BeginPlay();
	SetIsReplicated(true);
	GetOwnerCharacter();

	if (_ownerCharacter == nullptr)
	{
		DEBUG_STRING("OWNER CHARACTER NOT FOUND!");
		return;
	}

	LoadDefaultSlots();

	if (_ownerCharacter->IsLocallyControlled())
	{
		AssignInputActions();
	}

	ChangeSlot(_currentSlotIndex, true);
}

void UPW_InventoryHandler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPW_InventoryHandler::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPW_InventoryHandler, _inventorySlots);
	DOREPLIFETIME(UPW_InventoryHandler, _currentSlotIndex);
}

void UPW_InventoryHandler::LoadDefaultSlots()
{
	if (_ownerCharacter->HasAuthority())
		LocalLoadDefaultSlotsByID();
	else
		ServerLoadDefaultSlotsByID();
}

void UPW_InventoryHandler::ServerLoadDefaultSlotsByID_Implementation()
{
	if (!_ownerCharacter->HasAuthority())
		LocalLoadDefaultSlotsByID();
}

void UPW_InventoryHandler::LocalLoadDefaultSlotsByID()
{
	for (int i = 0; i < _defaultSlotTypes.Num(); i++)
		_inventorySlots.Add(FInventorySlot(_defaultSlotTypes[i]));
}

bool UPW_InventoryHandler::IsSlotValid(int slotIndex)
{
	return slotIndex >= 0 && slotIndex < _inventorySlots.Num();
}

void UPW_InventoryHandler::ChangeSlot(int targetedSlotIndex, bool forceChangeSlot)
{
	APW_ItemObject* currentItem = _inventorySlots[_currentSlotIndex].GetItem();
	
	if (currentItem != nullptr)
		OnInventorySlotChanged.Broadcast(currentItem);
	
	if (_currentSlotIndex == targetedSlotIndex && !forceChangeSlot)
		{ DEBUG_STRING("CURRENT SLOT IS THE SAME AS UPDATED SLOT!"); return; }

	if (!IsSlotValid(targetedSlotIndex))
		{ DEBUG_STRING("SLOT INDEX IS INVALID!"); return; }
	
	GetOwner()->HasAuthority() ? LocalChangeSlot(targetedSlotIndex) : SeverChangeSlot(targetedSlotIndex);
}

void UPW_InventoryHandler::SeverChangeSlot_Implementation(int targetedSlotIndex)
{
	if (!GetOwner()->HasAuthority()) return;
	LocalChangeSlot(targetedSlotIndex);
}

void UPW_InventoryHandler::LocalChangeSlot(int targetedSlotIndex)
{
	if (_currentSlotIndex != targetedSlotIndex)
	{
		APW_ItemObject* currentItem = _inventorySlots[_currentSlotIndex].GetItem();
	
		if (currentItem != nullptr)
			currentItem->DisableItem(_ownerCharacter);
	}

	APW_ItemObject* updatedItem = _inventorySlots[targetedSlotIndex].GetItem();
	
	if (updatedItem != nullptr)
		updatedItem->EnableItem(_ownerCharacter);
	
	_currentSlotIndex = targetedSlotIndex;
}

#pragma region CollectItem
void UPW_InventoryHandler::CollectItem(APW_ItemObject* collectedItem)
{
	if (collectedItem == nullptr)
		{ PW_Utilities::Log("COLLECTED ITEM IS NULL!"); return; }
	
	const EItemObjectState itemState = collectedItem->GetItemState();

	if (itemState != EItemObjectState::EDropped)
		{ PW_Utilities::Log("ITEM STATE IS NOT DROPPED! : " + collectedItem->GetName()); return; }

	const EItemType itemType = collectedItem->GetItemType();

	int slotIndex = 0;
	const bool foundSlot = TryGetSlotIndex(itemType, slotIndex);
	
	if (!foundSlot)
		{ PW_Utilities::Log("NO AVAILABLE SLOT!"); return; }
	
	GetOwner()->HasAuthority() ? LocalCollectItem(slotIndex, collectedItem) : ServerCollectItem(slotIndex, collectedItem);
}

void UPW_InventoryHandler::LocalCollectItem(int slotIndex, APW_ItemObject* collectedItem)
{
	if (collectedItem == nullptr)
		{ PW_Utilities::Log("COLLECTED ITEM IS NULL!"); return; }

	_inventorySlots[slotIndex].SetItem(collectedItem);

	_ownerCharacter = Cast<APW_Character>(GetOwner());
	
	USceneComponent* itemPosition  = _ownerCharacter->GetItemHolder();

	if (itemPosition == nullptr)
		{PW_Utilities::Log("ITEM POSITION IS NULL!"); return; }
	
	collectedItem->SetOwner(_ownerCharacter);
	collectedItem->UpdateItemState(EItemObjectState::EHeld);
	collectedItem->AttachToComponent(itemPosition, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	
	ChangeSlot(slotIndex, true);
}

void UPW_InventoryHandler::ServerCollectItem_Implementation(int slotIndex, APW_ItemObject* collectedItem)
{
	if (!_ownerCharacter->HasAuthority()) return;
		LocalCollectItem(slotIndex,collectedItem);
}
#pragma endregion CollectItem

bool UPW_InventoryHandler::TryGetSlotIndex(EItemType itemType, int& outIndex)
{
	for (int i = 0; i < _inventorySlots.Num(); i++)
	{
		if (_inventorySlots[i].GetSlotType() != itemType)
			continue;

		if (!_inventorySlots[i].IsSlotAvailable())
			continue;

		outIndex = i;
		return true;
	}
	return false;
}

void UPW_InventoryHandler::CycleNextSlot()
{
	const int targetedSlotIndex = _currentSlotIndex + 1;
	constexpr int overflowSlotIndex = 0;
	const bool isIndexValid = _inventorySlots.IsValidIndex(targetedSlotIndex);

	if (isIndexValid)
	{
		ChangeSlot(targetedSlotIndex);;
	}
	else
	{
		ChangeSlot(overflowSlotIndex);
	}
}

void UPW_InventoryHandler::CyclePreviousSlot()
{
	const int targetedSlotIndex = _currentSlotIndex - 1;
	const int overflowSlotIndex = _inventorySlots.Num() - 1;
	const bool isIndexValid = _inventorySlots.IsValidIndex(targetedSlotIndex);

	if (isIndexValid)
	{
		ChangeSlot(targetedSlotIndex);
	}
	else
	{
		ChangeSlot(overflowSlotIndex);
	}
}

void UPW_InventoryHandler::LoadItemsFromData(const FPlayerInventoryDataEntry& inventoryData)
{
	_ownerCharacter = Cast<APW_Character>(GetOwner());
	if (_ownerCharacter == nullptr) return;;
	_ownerCharacter->HasAuthority() ? LocalLoadFromData(inventoryData) : ServerLoadFromData(inventoryData);
}

void UPW_InventoryHandler::ServerLoadFromData_Implementation(const FPlayerInventoryDataEntry& inventoryData)
{
	if(!_ownerCharacter->HasAuthority()) return;;
	LocalLoadFromData(inventoryData);
}

void UPW_InventoryHandler::LocalLoadFromData(const FPlayerInventoryDataEntry& inventoryData)
{
	TArray<APW_ItemObject*> items = TArray<APW_ItemObject*>();
	for (int i = 0; i < inventoryData._itemIDs.Num(); i++)
	{
		TSubclassOf<APW_ItemObject> itemClass = GetItemObjectFromDataTable(inventoryData._itemIDs[i]);
		if (itemClass == nullptr)
		{ DEBUG_STRING("ITEM CLASS IS NULL!"); continue; }
		APW_ItemObject* item = GetWorld()->SpawnActor<APW_ItemObject>(itemClass, GetOwner()->GetActorLocation(), FRotator::ZeroRotator);
		items.Add(item);
		//DEBUG_STRING("Spawned Item");
	}

	int32 selectedIndex = inventoryData._selectedSlotIndex;

	//CollectItems(items);

	FTimerHandle itemTimer;
	FTimerDelegate itemDelegate;

	itemDelegate.BindLambda([selectedIndex,items, this]()
	{
		CollectItems(selectedIndex,items);
	});
	
	GetWorld()->GetTimerManager().SetTimer(itemTimer, itemDelegate, 1.0f, false);
}

void UPW_InventoryHandler::CollectItems(const int32 selectedIndex,  const TArray<APW_ItemObject*>& items)
{
	for ( int i = 0; i < items.Num(); i++)
	{
		APW_ItemObject* collectedItem = items[i];
		
		if (collectedItem == nullptr)
		{DEBUG_STRING("COLLECTED ITEM IS NULL!"); return;}
			
		const EItemType itemType = items[i]->GetItemType();

		int slotIndex = 0;
		const bool foundSlot = TryGetSlotIndex(itemType, slotIndex);

		if (!foundSlot)
		{DEBUG_STRING("NO AVAILABLE SLOT!"); return; }

		_inventorySlots[slotIndex].SetItem(collectedItem);

		_ownerCharacter = Cast<APW_Character>(GetOwner());
	
		USceneComponent* itemPosition  = _ownerCharacter->GetItemHolder();

		if (itemPosition == nullptr)
		{ DEBUG_STRING("ITEM POSITION IS NULL!"); return; }
	
		collectedItem->SetOwner(_ownerCharacter);
		collectedItem->UpdateItemState(EItemObjectState::EHeld);
		collectedItem->AttachToComponent(itemPosition, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		collectedItem->SetVisibility(false);
	}

	ChangeSlot(selectedIndex, true);
}

void UPW_InventoryHandler::DropAllItems()
{
	GetOwnerRole() == ROLE_Authority ? ServerDropAllItems() : LocalDropAllItems();
}

void UPW_InventoryHandler::ServerDropAllItems_Implementation()
{
	if (GetOwnerRole() == ROLE_Authority)
		LocalDropAllItems();
}

void UPW_InventoryHandler::LocalDropAllItems()
{
	for (int i = 0; i < _inventorySlots.Num(); i++)
		DropItem(i);
}

TArray<FString> UPW_InventoryHandler::GetInventoryItemIDs()
{
	TArray<FString> itemIDs = TArray<FString>();
	for (int i = 0; i < _inventorySlots.Num(); i++)
	{
		APW_ItemObject* item = _inventorySlots[i].GetItem();
		if (item == nullptr)
			continue;
		itemIDs.Add(item->GetItemID());
	}
	return itemIDs;
}

void UPW_InventoryHandler::CycleUp()
{
	if (_inventorySlots.IsEmpty())
		{ DEBUG_STRING("INVENTORY SLOTS ARE EMPTY, CANNOT CYCLE UP!"); return; }
	
	CycleNextSlot();
}

void UPW_InventoryHandler::CycleDown()
{
	if (_inventorySlots.IsEmpty())
		{ DEBUG_STRING("INVENTORY SLOTS ARE EMPTY, CANNOT CYCLE DOWN!"); return; }
	
	CyclePreviousSlot();
}

void UPW_InventoryHandler::ToSlot(int targetedSlotIndex)
{
	if (!IsSlotValid(targetedSlotIndex))
		{ DEBUG_STRING("SLOT INDEX IS INVALID!"); return; }

	ChangeSlot(targetedSlotIndex);
}

void UPW_InventoryHandler::DropCurrentItem()
{
	DropItem(_currentSlotIndex);
}

APW_ItemObject* UPW_InventoryHandler::GetCurrentItem()
{
	const int currentSlot = GetCurrentSlotIndex();
	APW_ItemObject* itemObject = _inventorySlots[currentSlot].GetItem();
	return itemObject;
}

void UPW_InventoryHandler::AssignInputActions()
{
	if (_ownerCharacter == nullptr)
		{ DEBUG_STRING("OWNER CHARACTER NOT FOUND!"); return; }
	
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
		{ DEBUG_STRING("OWNER ACTOR NOT FOUND!"); return; }

	_ownerCharacter = Cast<APW_Character>(ownerActor);
	
	if (_ownerCharacter == nullptr)
		{ DEBUG_STRING("OWNER CHARACTER NOT FOUND!"); }
}

TSubclassOf<APW_ItemObject> UPW_InventoryHandler::GetItemObjectFromDataTable(FString id)
{
	TSubclassOf<APW_ItemObject> itemClass = nullptr;
	if (_ItemDataTable)
	{
		FItems* itemData = _ItemDataTable->FindRow<FItems>(*id, "");
		if (itemData)
		{
			itemClass = itemData->_itemClass;
		}
	}
	return itemClass;
}

#pragma region DropItem
void UPW_InventoryHandler::DropItem(int slotIndex)
{
	if (!IsSlotValid(slotIndex))
		{ DEBUG_STRING("SLOT INDEX IS INVALID!"); return; }

	APW_ItemObject* slotItem = _inventorySlots[_currentSlotIndex].GetItem();

	if (slotItem == nullptr)
		{ DEBUG_STRING("SLOT IS EMPTY!"); return; }
	
	slotItem->RemoveActionBindings(_ownerCharacter);

	_ownerCharacter->HasAuthority() ? LocalDropItem(slotIndex) : ServerDropItem(slotIndex);
}

void UPW_InventoryHandler::LocalDropItem(int slotIndex)
{
	APW_ItemObject* slotItem = _inventorySlots[slotIndex].GetItem();
	
	if (slotItem == nullptr)
		{ DEBUG_STRING("[LOCAL] SLOT ITEM IS NULL!"); return; }

	_inventorySlots[slotIndex].RemoveItem();
	
	slotItem->UpdateItemState(EItemObjectState::EDropped);
	slotItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	slotItem->SetOwner(nullptr);

	if (_ownerCharacter == nullptr)
		{ DEBUG_STRING("[LOCAL] OWNER CHARACTER IS NULL!"); return; }
	
	const FVector characterVelocity = _ownerCharacter->GetVelocity();
	const FVector itemVelocity = characterVelocity * _throwVelocityMultiplier;

	UMeshComponent* itemMesh = slotItem->GetCollisionMesh();
	if (itemMesh == nullptr)
		{DEBUG_STRING("[LOCAL] ITEM MESH IS NULL!"); return; }

	itemMesh->AddImpulse(itemVelocity);
}

void UPW_InventoryHandler::ServerDropItem_Implementation(int slotIndex)
{
	if (_ownerCharacter->HasAuthority())
		LocalDropItem(slotIndex);
}

#pragma endregion DropItem
