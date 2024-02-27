// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_ActorMoverComponent.h"

// Sets default values for this component's properties
UPW_ActorMoverComponent::UPW_ActorMoverComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	_moveTime = 1.0f;
	_canMove = false;
	_moveDirection = EMoveDirection::EMD_Forward;
}


// Called when the game starts
void UPW_ActorMoverComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPW_ActorMoverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	HandleMovement(DeltaTime);
}

void UPW_ActorMoverComponent::SetPoints(FVector startPoint, FVector endPoint)
{
	if (startPoint.Equals(endPoint)) return;
	_startPoint = startPoint;
	_endPoint = endPoint;
}

void UPW_ActorMoverComponent::HandleMovement(float DeltaTime)
{
	AActor* owner = GetOwner();
	if (owner && owner->HasAuthority() && _canMove)
	{
		FVector currentLocation = owner->GetActorLocation();
		FVector targetLocation = _moveDirection == EMoveDirection::EMD_Forward ? _endPoint : _startPoint;
		
		if (!currentLocation.Equals(targetLocation))
		{
			float speed  = FVector::Dist(_startPoint, _endPoint) /_moveTime;
			FVector newLocation = FMath::VInterpConstantTo(currentLocation, targetLocation, DeltaTime, speed);
			owner->SetActorLocation(newLocation);
		}
	}
}
