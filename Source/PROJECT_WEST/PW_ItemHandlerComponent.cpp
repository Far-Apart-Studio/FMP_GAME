// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_ItemHandlerComponent.h"
#include "PROJECT_WEST/Items/PW_Item.h"
#include "PROJECT_WEST/PW_Character.h"

// Sets default values for this component's properties
UPW_ItemHandlerComponent::UPW_ItemHandlerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPW_ItemHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPW_ItemHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPW_ItemHandlerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UPW_ItemHandlerComponent::SetOverlappingItem(APW_Item* Item)
{
	if (_overlappingItem)
	{
		_overlappingItem->ShowPickupWidget(false);
	}
	_overlappingItem = Item;

	APW_Character* character = Cast<APW_Character>(GetOwner());
	if (character && character->IsLocallyControlled())
	{
		if (_overlappingItem)
		{
			_overlappingItem->ShowPickupWidget(true);
		}
	}
}

void UPW_ItemHandlerComponent::DoPickUp()
{
	ServerPickUp();
}

void UPW_ItemHandlerComponent::DoSwitchItem()
{
	if (_currentItemIndex >= _itemsInInventory.Num()) _currentItemIndex = -1;
	_currentItemIndex = (_currentItemIndex + 1) % _itemsInInventory.Num();
	ServerEquip();
}

void UPW_ItemHandlerComponent::DoDrop()
{
	ServerDrop();
}

void UPW_ItemHandlerComponent::EquipItem(APW_Item* Apw_Item)
{
	if (_itemInHand)
	{
		_itemInHand->SetVisibility( false );
	}
	
	_itemInHand = Apw_Item;
	_itemInHand->SetVisibility( false );
	_itemInHand->SetItemState(EItemState::EIS_Equipped);
	_itemInHand->SetOwner(GetOwner());
	_itemInHand->AttachToComponent(_itemHolder, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void UPW_ItemHandlerComponent::UnEquipItem(APW_Item* Apw_Item)
{
	_itemInHand->SetVisibility( false );
}

void UPW_ItemHandlerComponent::DropItem()
{
	if(!_itemInHand) return;

	_itemsInInventory.Remove(_itemInHand);
	
	_itemInHand->SetItemState(EItemState::EIS_Dropped);
	_itemInHand->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	_itemInHand->SetOwner(nullptr);
	_itemInHand = nullptr;
}

void UPW_ItemHandlerComponent::OnRep_ItemChange(APW_Item* lastItem)
{
	if (lastItem)
	{
		lastItem->SetItemState(EItemState::EIS_Dropped);
		lastItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		lastItem->SetOwner(nullptr);
	}
	
	if (_itemInHand)
	{
		_itemInHand->SetItemState(EItemState::EIS_Equipped);
		_itemInHand->SetOwner(GetOwner());
		_itemInHand->AttachToComponent(_itemHolder, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
}

void UPW_ItemHandlerComponent::OnRep_OverlappinItem(APW_Item* lastItem)
{
	if (_overlappingItem)
	{
		_overlappingItem->ShowPickupWidget(true);
	}
	
	if (lastItem)
	{
		lastItem->ShowPickupWidget(false);
	}
}

void UPW_ItemHandlerComponent::ServerPickUp_Implementation()
{
	if (_overlappingItem)
	{
		if (_itemInHand)
		{
			//ServerDrop();
		}

		_itemsInInventory.Add(_overlappingItem);
		EquipItem(_overlappingItem);
		_overlappingItem = nullptr;
	}
}

void UPW_ItemHandlerComponent::ServerEquip_Implementation()
{
	APW_Item* item = _itemsInInventory[_currentItemIndex];
	if (item)
	{
		EquipItem(item);
	}
}

void UPW_ItemHandlerComponent::ServerDrop_Implementation()
{
	if (_itemInHand)
	{
		DropItem();
	}
}


