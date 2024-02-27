// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_BoxSpawningComponent.h"

#include "Components/BoxComponent.h"
#include "PROJECT_WEST/DebugMacros.h"

UPW_BoxSpawningComponent::UPW_BoxSpawningComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	_boxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	_boxComponent->SetupAttachment(this);
}

// Called when the game starts
void UPW_BoxSpawningComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
}

// Called every frame
void UPW_BoxSpawningComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

AActor* UPW_BoxSpawningComponent::SpawnActorInBox(TSubclassOf<AActor> actorClass)
{
	FVector position, normal;
	GetGroundPositionAndNormal(position, normal);
	AActor* actor = GetWorld()->SpawnActor<AActor> (actorClass, position, normal.Rotation());
	return actor;
}

FVector UPW_BoxSpawningComponent::GetRandomPositionInBox()
{
	FVector result = FVector::ZeroVector;
	if (_boxComponent)
	{
		FVector origin = _boxComponent->GetComponentLocation();
		FVector extent = _boxComponent->GetScaledBoxExtent();
		result = FMath::RandPointInBox(FBox(origin - extent, origin + extent));
	}
	return result;
}

bool UPW_BoxSpawningComponent::IsPositionValid(FVector position)
{
	FHitResult hit;
	FCollisionQueryParams params;
	params.AddIgnoredActor(GetOwner());
	return !GetWorld()->SweepSingleByChannel(hit, position, position, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(50.0f), params);
}

void UPW_BoxSpawningComponent::GetGroundPositionAndNormal(FVector& outPosition, FVector& outNormal)
{
	FHitResult hit;
	FVector start = GetRandomPositionInBox();
	FVector end = start - FVector::UpVector * 1000.0f;
	const float outPositionZOffset = 100.f;
	
	FCollisionQueryParams params;
	params.AddIgnoredActor(GetOwner());
	if (GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Visibility, params))
	{
		outPosition = hit.ImpactPoint + FVector::UpVector * outPositionZOffset;
		outNormal = hit.ImpactNormal;
		
		DRAW_LINE(start, outPosition);
		DRAW_SPHERE(hit.ImpactPoint);
	}
	else
	{
		outPosition = start;
		outNormal = FVector::UpVector;
	}
}

