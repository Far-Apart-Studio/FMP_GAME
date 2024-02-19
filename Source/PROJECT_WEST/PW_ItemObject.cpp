// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_ItemObject.h"

#include "PW_Character.h"
#include "PW_InventoryHandler.h"
#include "PW_Utilities.h"


APW_ItemObject::APW_ItemObject()
{
	PrimaryActorTick.bCanEverTick = true;
	_itemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	_itemMesh->SetIsReplicated(true);
	SetRootComponent(_itemMesh);

	_itemMesh->SetCollisionResponseToAllChannels(ECR_Block);
	_itemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	_itemMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	_itemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APW_ItemObject::BeginPlay()
{
	Super::BeginPlay();
}

void APW_ItemObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APW_ItemObject::UpdateItemState(EItemObjectState updatedState)
{
	switch (updatedState)
	{
		case EItemObjectState::EHeld:
			EnterHeldState();
			break;
		case EItemObjectState::EDropped:
			EnterDroppedState();
			break;
	}

	_itemState = updatedState;
}

void APW_ItemObject::UpdateItemType(EItemType updatedType)
{
	_itemType = updatedType;
}

void APW_ItemObject::SetVisibility(bool isVisible)
{
	if (_itemMesh == nullptr)
		{ PW_Utilities::Log("ITEM MESH IS NULL!"); return; }

	_itemMesh->SetVisibility(isVisible);
}

void APW_ItemObject::StartFocus_Implementation()
{
	
}

void APW_ItemObject::EndFocus_Implementation()
{
	
}

void APW_ItemObject::StartInteract_Implementation(AActor* owner)
{
	PW_Utilities::Log("ITEM OBJECT: START INTERACT");
	
	const APW_Character* characterController = Cast<APW_Character>(owner);

	if (characterController == nullptr)
		{ PW_Utilities::Log("ITEM OBJECT: OWNER IS NULL!"); return; }
	
	UActorComponent* actorComponent = characterController->GetComponentByClass(UPW_InventoryHandler::StaticClass());
	UPW_InventoryHandler* inventoryHandler = Cast<UPW_InventoryHandler>(actorComponent);

	if (inventoryHandler == nullptr)
		{ PW_Utilities::Log("ITEM OBJECT: INVENTORY HANDLER IS NULL!"); return; }
	
	if (characterController->IsLocallyControlled())
		inventoryHandler->TryCollectItem(this);
}

void APW_ItemObject::EnterHeldState()
{
	if (_itemMesh == nullptr)
		{ PW_Utilities::Log("ITEM MESH IS NULL!"); return; }
	
	_itemMesh->SetSimulatePhysics(false);
	_itemMesh->SetEnableGravity(false);
	_itemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

