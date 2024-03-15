// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_ThrowableComponent.h"

#include "FDashAction.h"
#include "PW_ItemObject.h"

UPW_ThrowableComponent::UPW_ThrowableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	_throwableItemObject = nullptr;
}

void UPW_ThrowableComponent::BeginPlay()
{
	Super::BeginPlay();
	InitialiseComponent();
}

void UPW_ThrowableComponent::InitialiseComponent()
{
	AActor* ownerActor = GetOwner();
	if (ownerActor)
	{
		_throwableItemObject = Cast<APW_ItemObject>(ownerActor);
		_throwableItemObject->OnApplyInputBindingDelegate.AddDynamic(this, &UPW_ThrowableComponent::SetThrowableInputBinding);
		_throwableItemObject->OnRemoveInputBindingDelegate.AddDynamic(this, &UPW_ThrowableComponent::RemoveThrowableInputBinding);
	}
}

void UPW_ThrowableComponent::QueueThrow()
{
	OnQueueThrow.Broadcast();
}

void UPW_ThrowableComponent::CancelThrow()
{
	OnCancelThrow.Broadcast();	
}

void UPW_ThrowableComponent::Throw()
{
	OnThrow.Broadcast();
	
	_throwableItemObject->UpdateItemState(EItemObjectState::EDropped);
	UStaticMeshComponent* itemMesh = _throwableItemObject->GetItemMesh();
	const AActor* objectThrower = _throwableItemObject->GetOwner();

	if (objectThrower)
	{
		const FVector throwDirection = objectThrower->GetActorForwardVector();
		const FVector throwImpulse = throwDirection * _throwForce;
		itemMesh->AddImpulse(throwImpulse);
	}
}

void UPW_ThrowableComponent::SetThrowableInputBinding(APW_Character* characterOwner)
{
	
}

void UPW_ThrowableComponent::RemoveThrowableInputBinding(APW_Character* characterOwner)
{
	
}

void UPW_ThrowableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

