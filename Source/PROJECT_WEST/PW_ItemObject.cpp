// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_ItemObject.h"

#include "PW_Character.h"
#include "PW_InventoryHandler.h"
#include "PW_Utilities.h"
#include "Net/UnrealNetwork.h"
#include "PROJECT_WEST/PW_Character.h"


APW_ItemObject::APW_ItemObject()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	_itemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	_itemMesh->SetIsReplicated(true);
	
	SetRootComponent(_itemMesh);

	_itemMesh->SetSimulatePhysics(true);
	_itemMesh->SetEnableGravity(true);
	_itemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	_itemMesh->SetCollisionResponseToAllChannels(ECR_Block);
	_itemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	_itemMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void APW_ItemObject::BeginPlay()
{
	Super::BeginPlay();

	SetReplicates(true);
	SetReplicateMovement(true);
	AttachToOwner();
}

void APW_ItemObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APW_ItemObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APW_ItemObject, _itemState);
	DOREPLIFETIME(APW_ItemObject, _isVisible);
	DOREPLIFETIME(APW_ItemObject, _isActive);
	DOREPLIFETIME(APW_ItemObject, _itemMesh);
}

void APW_ItemObject::UpdateItemType(EItemType updatedType)
{
	_itemType = updatedType;
}

void APW_ItemObject::StartFocus_Implementation()
{
	
}

void APW_ItemObject::EndFocus_Implementation()
{
	
}

void APW_ItemObject::StartInteract_Implementation(AActor* owner)
{
	DEBUG_STRING("START INTERACT");
	
	const APW_Character* characterController = Cast<APW_Character>(owner);

	if (characterController == nullptr)
		{ PW_Utilities::Log("ITEM OBJECT: OWNER IS NULL!"); return; }
	
	UActorComponent* actorComponent = characterController->GetComponentByClass(UPW_InventoryHandler::StaticClass());
	UPW_InventoryHandler* inventoryHandler = Cast<UPW_InventoryHandler>(actorComponent);

	if (inventoryHandler == nullptr)
		{ PW_Utilities::Log("ITEM OBJECT: INVENTORY HANDLER IS NULL!"); return; }
	
	if (characterController->IsLocallyControlled())
		inventoryHandler->CollectItem(this);
}

void APW_ItemObject::EnterHeldState()
{
	if (_itemMesh == nullptr)
		{ PW_Utilities::Log("ITEM MESH IS NULL!"); return; }
	
	_itemMesh->SetSimulatePhysics(false);
	_itemMesh->SetEnableGravity(false);
	_itemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_itemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void APW_ItemObject::EnterDroppedState()
{
	if (_itemMesh == nullptr)
		{ PW_Utilities::Log("ITEM MESH IS NULL!"); return; }
	
	const FDetachmentTransformRules detachmentRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld,
	EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
	_itemMesh->DetachFromComponent(detachmentRules);
	_itemMesh->SetSimulatePhysics(true);
	_itemMesh->SetEnableGravity(true);
	_itemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	_itemMesh->SetCollisionResponseToAllChannels(ECR_Block);
	_itemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	_itemMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void APW_ItemObject::AttachToOwner()
{
	if(GetOwner())
	{
		APW_Character* characterOwner = Cast<APW_Character>(GetOwner());
		if(characterOwner)
		{
			if(characterOwner)
			{
				AttachToComponent( characterOwner->GetItemHolder(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				OnSetVisibility();
			}
		}
	}
}

void APW_ItemObject::OnRep_ItemStateChanged()
{
	OnUpdateItemState();
}

void APW_ItemObject::OnRep_VisibilityChange()
{
	OnSetVisibility();
}

#pragma region SetVisibility
void APW_ItemObject::SetVisibility(bool isVisible)
{
	_isVisible = isVisible;
	OnSetVisibility();
}

void APW_ItemObject::OnSetVisibility()
{
	if (_itemMesh == nullptr)
	{ PW_Utilities::Log("ITEM MESH IS NULL!"); return; }

	TArray<USceneComponent*> childMeshes;
	_itemMesh->GetChildrenComponents(true, childMeshes);
	
	for (USceneComponent* childMesh : childMeshes)
		childMesh->SetVisibility(_isVisible);

	_isActive = _isVisible;
}

#pragma endregion SetVisibility

#pragma region UpdateItemState

void APW_ItemObject::UpdateItemState(EItemObjectState updatedState)
{
	_itemState = updatedState;
	OnUpdateItemState();
}

void APW_ItemObject::OnUpdateItemState()
{
	switch (_itemState)
	{
	case EItemObjectState::EHeld:
		EnterHeldState();
		break;
	case EItemObjectState::EDropped:
		EnterDroppedState();
		break;
	}
}

#pragma endregion UpdateItemStateNetworked

#pragma region ApplyActionBindings

void APW_ItemObject::ApplyActionBindings(APW_Character* characterOwner)
{
	HasAuthority() ? ClientApplyActionBindings(characterOwner) : LocalApplyActionBindings(characterOwner);
}

void APW_ItemObject::ClientApplyActionBindings_Implementation(APW_Character* characterOwner)
{
	if (characterOwner == nullptr)
		{ PW_Utilities::Log("CHARACTER OWNER IS NULL!"); return; }

	if (characterOwner->IsLocallyControlled())
		LocalApplyActionBindings(characterOwner);
}

void APW_ItemObject::LocalApplyActionBindings(APW_Character* characterOwner)
{
	DEBUG_STRING("APPLY BINDING ACTIONS");
}
#pragma endregion ApplyActionBindings

#pragma region RemoveActionBindings

void APW_ItemObject::RemoveActionBindings(APW_Character* characterOwner)
{
	HasAuthority() ? ClientRemoveActionBindings(characterOwner) : LocalRemoveActionBindings(characterOwner);
}

void APW_ItemObject::ClientRemoveActionBindings_Implementation(APW_Character* characterOwner)
{
	if (characterOwner == nullptr)
		{ PW_Utilities::Log("CHARACTER OWNER IS NULL!"); return; }

	if (characterOwner->IsLocallyControlled())
		LocalRemoveActionBindings(characterOwner);
}

void APW_ItemObject::LocalRemoveActionBindings(APW_Character* characterOwner)
{
	DEBUG_STRING("REMOVE BINDING ACTIONS");
}

#pragma endregion RemoveActionBindings

void APW_ItemObject::EnableItem(APW_Character* characterOwner)
{
	ApplyActionBindings(characterOwner);
	SetVisibility(true);
}

void APW_ItemObject::DisableItem(APW_Character* characterOwner)
{
	RemoveActionBindings(characterOwner);
	SetVisibility(false);
}

