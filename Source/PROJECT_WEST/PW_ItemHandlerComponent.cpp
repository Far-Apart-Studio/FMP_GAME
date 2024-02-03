// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_ItemHandlerComponent.h"
#include "PROJECT_WEST/Items/PW_Item.h"
#include "PROJECT_WEST/PW_Character.h"
#include "DebugMacros.h"
#include "Net/UnrealNetwork.h"

UPW_ItemHandlerComponent::UPW_ItemHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UPW_ItemHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
	SetIsReplicated( true );

	_ownerCharacter = _ownerCharacter ?   Cast<APW_Character>(GetOwner()) : _ownerCharacter;
	if (_ownerCharacter->IsLocallyControlled())
	{
		_ownerCharacter->OnPickUpButtonPressed.AddDynamic(this, &UPW_ItemHandlerComponent::TryPickUpItemNearBy);
		_ownerCharacter->OnDropButtonPressed.AddDynamic(this, &UPW_ItemHandlerComponent::TryDropItemHeld);
		_ownerCharacter->OnSwitchItemButtonPressed.AddDynamic(this, &UPW_ItemHandlerComponent::DoSwitchItem);
	}
}

void UPW_ItemHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UPW_ItemHandlerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(UPW_ItemHandlerComponent, _overlappingItem);
	DOREPLIFETIME(UPW_ItemHandlerComponent, _itemInHand);
	DOREPLIFETIME(UPW_ItemHandlerComponent, _itemsInInventory);
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

void UPW_ItemHandlerComponent::TryPickUpItemNearBy()
{
	DoPickUp(_overlappingItem);
}

void UPW_ItemHandlerComponent::DoSwitchItem()
{
	if  ( _itemsInInventory.Num() == 0 ) return;
	
	if (_itemInHand)
	{
		if (GetOwner()->HasAuthority())
		{
			UnEquipItem(_itemInHand);
		}
		else
		{
			ServerUnEquip(_itemInHand);
		}
	}

	int currentIndex = _itemsInInventory.Find(_itemInHand);
	currentIndex = (currentIndex + 1) % _itemsInInventory.Num();
	_itemInHand = _itemsInInventory[currentIndex];

	if (_itemInHand)
	{
		if (GetOwner()->HasAuthority())
		{
			EquipItem(_itemInHand);
		}
		else
		{
			ServerEquip(_itemInHand);
		}
	}
}

void UPW_ItemHandlerComponent::DoPickUp(APW_Item* item)
{
	if (GetOwner()->HasAuthority())
	{
		DEBUG_STRING( "HasAuthority : PICKING UP ITEM!" );
		PickUpItem(item);
	}
	else
	{
		DEBUG_STRING( "NoAuthority : PICKING UP ITEM!" );
		ServerPickUp(item);
	}
}

void UPW_ItemHandlerComponent::DoEquip(APW_Item* item)
{
	if (GetOwner()->HasAuthority())
	{
		EquipItem(item);
	}
	else
	{
		ServerEquip(item);
	}
}

void UPW_ItemHandlerComponent::TryDropItemHeld()
{
	if (GetOwner()->HasAuthority())
	{
		DropItem(_itemInHand);
	}
	else
	{
		ServerDrop( _itemInHand );
	}
}

void UPW_ItemHandlerComponent::PickUpItem(APW_Item* item)
{
	if (!item)
	{
		DEBUG_STRING( "PickUpItem : item is nullptr!" );
		return;
	}
	
	if (_itemInHand)
	{
		DEBUG_STRING( "PickUpItem : UnEquipItem!" );
		UnEquipItem( _itemInHand );
	}
	
	_itemsInInventory.Add(item);
	_itemInHand = item;
	_itemInHand->SetOwner(GetOwner());
	_itemInHand->SetItemState(EItemState::EIS_Pickup);
	_itemInHand->AttachToComponent(Cast<APW_Character>(GetOwner())->GetItemHolder(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	DEBUG_STRING( "PickUpItem : _itemInHand : " + Cast<APW_Character>(GetOwner())->GetItemHolder()->GetName() );
}

void UPW_ItemHandlerComponent::EquipItem(APW_Item* item)
{
	//item->OnVisibilityChange( true );

	if(!item) return;

	if (GetOwner()->HasAuthority())
	{
		item->OnVisibilityChange( true );
	}
	
	item->SetVisibility( true );
}

void UPW_ItemHandlerComponent::UnEquipItem(APW_Item* item)
{
	//item->OnVisibilityChange( false );

	if(!item) return;
	
	if (GetOwner()->HasAuthority())
	{
		item->OnVisibilityChange( false );
	}
	
	item->SetVisibility( false );
}

void UPW_ItemHandlerComponent::DropItem(APW_Item* item)
{
	if(!item) return;
	_itemsInInventory.Remove(item);
	item->SetItemState( EItemState::EIS_Dropped );

	if (_itemsInInventory.Num() > 0)
	{
		_itemInHand = _itemsInInventory[0];
		EquipItem(_itemInHand);
	}
	else
	{
		_itemInHand = nullptr;
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

void UPW_ItemHandlerComponent::ServerPickUp_Implementation(APW_Item* item)
{
	if (!GetOwner()->HasAuthority()) return;
	
	PickUpItem(item);
	_overlappingItem = nullptr;
}

void UPW_ItemHandlerComponent::ServerEquip_Implementation(APW_Item* item)
{
	if (!GetOwner()->HasAuthority()) return;
	EquipItem( item );
}

void UPW_ItemHandlerComponent::ServerUnEquip_Implementation(APW_Item* item)
{
	if (!GetOwner()->HasAuthority()) return;
	UnEquipItem( item );
}

void UPW_ItemHandlerComponent::ServerDrop_Implementation(APW_Item* item)
{
	if (!GetOwner()->HasAuthority()) return;
	DropItem(item);
}


