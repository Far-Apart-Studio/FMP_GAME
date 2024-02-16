// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_ItemObject.h"


APW_ItemObject::APW_ItemObject()
{
	PrimaryActorTick.bCanEverTick = true;
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

void APW_ItemObject::StartFocus_Implementation()
{
	IPW_InteractableInterface::StartFocus_Implementation();
}

void APW_ItemObject::EndFocus_Implementation()
{
	IPW_InteractableInterface::EndFocus_Implementation();
}

void APW_ItemObject::StartInteract_Implementation(AActor* owner)
{
	IPW_InteractableInterface::StartInteract_Implementation(owner);
}

void APW_ItemObject::EndInteract_Implementation()
{
	IPW_InteractableInterface::EndInteract_Implementation();
}

bool APW_ItemObject::IsInteracting_Implementation()
{
	return IPW_InteractableInterface::IsInteracting_Implementation();
}

void APW_ItemObject::EnterHeldState()
{
	_itemMesh->SetSimulatePhysics(false);
	_itemMesh->SetEnableGravity(false);
	_itemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APW_ItemObject::EnterDroppedState()
{
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

