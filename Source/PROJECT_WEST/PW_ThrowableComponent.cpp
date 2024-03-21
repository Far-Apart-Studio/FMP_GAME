// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_ThrowableComponent.h"
#include "FDashAction.h"
#include "PW_InventoryHandler.h"
#include "PW_ItemObject.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

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
		_throwableItemObject->OnApplyInputBinding.AddDynamic(this, &UPW_ThrowableComponent::SetThrowableInputBinding);
		_throwableItemObject->OnRemoveInputBinding.AddDynamic(this, &UPW_ThrowableComponent::RemoveThrowableInputBinding);
		_throwableItemObject->OnEnterDroppedState.AddDynamic(this, &UPW_ThrowableComponent::ReleaseThrowable);
	}
}

void UPW_ThrowableComponent::QueueThrow()
{
	OnQueueThrow.Broadcast();
	_canThrow = true;

	FTimerManager& timerManager = GetWorld()->GetTimerManager();
	timerManager.SetTimer(_throwTimerHandle, this,
		&UPW_ThrowableComponent::DrawTrajectory, _drawTrajectoryTime, true);
}

void UPW_ThrowableComponent::CancelThrow()
{
	OnCancelThrow.Broadcast();
	_canThrow = false;
	
	FTimerManager& timerManager = GetWorld()->GetTimerManager();
	timerManager.ClearTimer(_throwTimerHandle);
}

void UPW_ThrowableComponent::Throw()
{
	OnThrow.Broadcast();

	if (!_canThrow)
		return;
	
	UStaticMeshComponent* itemMesh = _throwableItemObject->GetItemMesh();
	const AActor* objectThrower = _throwableItemObject->GetOwner();

	UPW_InventoryHandler* currentInventory = _throwableItemObject->GetHolderInventory();
	if (currentInventory)
		currentInventory->DropCurrentItem();

	if (objectThrower)
	{
		const UCameraComponent* actorCamera = Cast<UCameraComponent>(objectThrower->GetComponentByClass(UCameraComponent::StaticClass()));
		if (actorCamera)
		{
			const FVector throwDirection = actorCamera->GetForwardVector();
			const FVector throwImpulse = throwDirection * _throwVelocity;
			itemMesh->AddImpulse(throwImpulse);
		}
		else
		{
			const FVector throwDirection = objectThrower->GetActorForwardVector();
			const FVector throwImpulse = throwDirection * _throwVelocity;
			itemMesh->AddImpulse(throwImpulse);
		}
	}
}

void UPW_ThrowableComponent::ReleaseThrowable()
{
	_canThrow = false;
	FTimerManager& timerManager = GetWorld()->GetTimerManager();
	timerManager.ClearTimer(_throwTimerHandle);
}

void UPW_ThrowableComponent::DrawTrajectory()
{
	AActor* ownerActor = GetOwner();
	FVector startLocation = ownerActor->GetActorLocation();
	FVector launchVelocity = ownerActor->GetActorForwardVector() * _throwVelocity;
	
	FPredictProjectilePathParams predictParams;
	FPredictProjectilePathResult predictResult;

	predictParams.StartLocation = startLocation;
	predictParams.LaunchVelocity = launchVelocity;
	predictParams.MaxSimTime = 3.0f;
	predictParams.SimFrequency = 10.0f;
	predictParams.DrawDebugType = EDrawDebugTrace::ForOneFrame;
	
	UGameplayStatics::PredictProjectilePath(this, predictParams, predictResult);
}

void UPW_ThrowableComponent::SetThrowableInputBinding(APW_Character* characterOwner)
{
	characterOwner->OnAimButtonPressed.AddDynamic(this, &UPW_ThrowableComponent::QueueThrow);
	characterOwner->OnAimButtonReleased.AddDynamic(this, &UPW_ThrowableComponent::CancelThrow);
	characterOwner->OnShootButtonPressed.AddDynamic(this, &UPW_ThrowableComponent::Throw);
}

void UPW_ThrowableComponent::RemoveThrowableInputBinding(APW_Character* characterOwner)
{
	characterOwner->OnAimButtonPressed.RemoveDynamic(this, &UPW_ThrowableComponent::QueueThrow);
	characterOwner->OnAimButtonReleased.RemoveDynamic(this, &UPW_ThrowableComponent::CancelThrow);
	characterOwner->OnShootButtonPressed.RemoveDynamic(this, &UPW_ThrowableComponent::Throw);
}

void UPW_ThrowableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

