// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_ItemObject.h"

#include "DebugMacros.h"
#include "PW_Character.h"
#include "PW_InventoryHandler.h"
#include "PW_Utilities.h"
#include "Gameplay/Components/PW_HighlightCompont.h"
#include "Net/UnrealNetwork.h"

APW_ItemObject::APW_ItemObject()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	_itemCollisionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollisionMesh"));
	SetRootComponent(_itemCollisionMesh);

	_itemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	_itemMesh->SetupAttachment(_itemCollisionMesh);
	
	_itemCollisionMesh->SetSimulatePhysics(true);
	_itemCollisionMesh->SetEnableGravity(true);
	_itemCollisionMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	_itemCollisionMesh->SetCollisionResponseToAllChannels(ECR_Block);
	_itemCollisionMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	_itemCollisionMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	_highlightComponent = CreateDefaultSubobject<UPW_HighlightCompont>(TEXT("HighlightComponent"));
}

void APW_ItemObject::BeginPlay()
{
	Super::BeginPlay();

	SetReplicates(true);
	SetReplicateMovement(true);
	AttemptAttachToOwner();
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
}

void APW_ItemObject::UpdateItemType(EItemType updatedType)
{
	_itemType = updatedType;
}

void APW_ItemObject::StartFocus_Implementation()
{
	if (_itemState != EItemObjectState::EDropped) return;
	_highlightComponent->ShowHighlight();
}

void APW_ItemObject::EndFocus_Implementation()
{
	_highlightComponent->HideHighlight();
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

UPW_InventoryHandler* APW_ItemObject::GetHolderInventory()
{
	AActor* ownerActor = GetOwner();
	
	if (ownerActor == nullptr)
		{ PW_Utilities::Log("OWNER IS NULL!"); return nullptr; }
	
	const APW_Character* characterOwner = Cast<APW_Character>(ownerActor);
	
	if (characterOwner == nullptr)
		{ PW_Utilities::Log("CHARACTER OWNER IS NULL!"); return nullptr; }
	
	UActorComponent* actorComponent = characterOwner->GetComponentByClass(UPW_InventoryHandler::StaticClass());
	UPW_InventoryHandler* inventoryHandler = Cast<UPW_InventoryHandler>(actorComponent);

	if (inventoryHandler == nullptr)
		{ PW_Utilities::Log("INVENTORY HANDLER IS NULL!"); return nullptr; }

	return inventoryHandler;
}

void APW_ItemObject::EnterHeldState()
{
	if (_itemCollisionMesh == nullptr)
		{ PW_Utilities::Log("ITEM MESH IS NULL!"); return; }

	OnEnterHeldState.Broadcast();
	
	_itemCollisionMesh->SetSimulatePhysics(false);
	_itemCollisionMesh->SetEnableGravity(false);
	_itemCollisionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_itemCollisionMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void APW_ItemObject::EnterDroppedState()
{
	if (_itemCollisionMesh == nullptr)
		{ PW_Utilities::Log("ITEM MESH IS NULL!"); return; }

	OnEnterDroppedState.Broadcast();
	
	const FDetachmentTransformRules detachmentRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld,
	EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
	_itemCollisionMesh->DetachFromComponent(detachmentRules);
	_itemCollisionMesh->SetSimulatePhysics(true);
	_itemCollisionMesh->SetEnableGravity(true);
	_itemCollisionMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	_itemCollisionMesh->SetCollisionResponseToAllChannels(ECR_Block);
	_itemCollisionMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	_itemCollisionMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void APW_ItemObject::AttemptAttachToOwner()
{
	AActor* ownerActor = GetOwner();

	if (ownerActor == nullptr)
		{ PW_Utilities::Log("OWNER IS NULL!"); return; }
	
	const APW_Character* characterOwner = Cast<APW_Character>(ownerActor);

	if (characterOwner)
	{
		AttachToComponent( characterOwner->GetItemHolder(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		OnSetVisibility();
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
	if (_itemCollisionMesh == nullptr)
	{ PW_Utilities::Log("ITEM MESH IS NULL!"); return; }

	TArray<USceneComponent*> childMeshes;
	_itemCollisionMesh->GetChildrenComponents(true, childMeshes);
	
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

	if (_itemState == EItemObjectState::EHeld)
	{
		_onPickedUpServer.Broadcast(this);
	}
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
	OnApplyInputBinding.Broadcast(characterOwner);
	DEBUG_STRING("LOCAL APPLY ACTION BINDINGS");
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
	OnRemoveInputBinding.Broadcast(characterOwner);
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

