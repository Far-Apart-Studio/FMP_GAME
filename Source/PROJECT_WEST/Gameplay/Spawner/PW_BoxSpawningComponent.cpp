// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_BoxSpawningComponent.h"

#include "Components/BoxComponent.h"
#include "PROJECT_WEST/DebugMacros.h"

UPW_BoxSpawningComponent::UPW_BoxSpawningComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	_spawnHeightOffset = 100.0f;
}

void UPW_BoxSpawningComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPW_BoxSpawningComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
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

FVector UPW_BoxSpawningComponent::GetRandomPositionInBoxEdge()
{
	FVector result = FVector::ZeroVector;
	if (_boxComponent)
	{
		FVector origin = _boxComponent->GetComponentLocation();
		FVector extent = _boxComponent->GetScaledBoxExtent();
		FVector min = origin - extent;
		FVector max = origin + extent;
		FVector edge = FVector(FMath::RandRange(min.X, max.X), FMath::RandRange(min.Y, max.Y), FMath::RandRange(min.Z, max.Z));
		result = edge;
	}
	return result;
}

bool UPW_BoxSpawningComponent::GetGroundPositionAndNormal(FVector origin,FVector& outPosition)
{
	FHitResult hit;
	FVector start = origin;
	FVector end = start - FVector::UpVector * 5000.0f;
	
	FCollisionQueryParams params;
	params.AddIgnoredActor(GetOwner());
	if (GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Visibility, params))
	{
		outPosition = hit.ImpactPoint + FVector::UpVector * _spawnHeightOffset;
		//DRAW_LINE_SingleFrame(start, outPosition);
		//DRAW_SPHERE_SingleFrame(hit.ImpactPoint);
		return true;
	}
	else
	{
		outPosition = start;
		return false;
	}
}

