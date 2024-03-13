// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_SpawnPointsHandlerComponent.h"

// Sets default values for this component's properties
UPW_SpawnPointsHandlerComponent::UPW_SpawnPointsHandlerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPW_SpawnPointsHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPW_SpawnPointsHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FVector UPW_SpawnPointsHandlerComponent::GetLanternSpawnPoint()
{
	return ConvertToWorldLocation(_lanternSpawnPoint.GetRandomSpawnPoint());
}

FVector UPW_SpawnPointsHandlerComponent::GetBountyPortalSpawnPoint()
{
	return ConvertToWorldLocation(_bountyPortalSpawnPoint.GetRandomSpawnPoint());
}

FVector UPW_SpawnPointsHandlerComponent::GetExtractionSpawnPoint()
{
	return ConvertToWorldLocation(_extractionPoint.GetRandomSpawnPoint());
}

TArray<FVector> UPW_SpawnPointsHandlerComponent::GetEnemySpawnPoint()
{
	return _enemySpawnPoints.spawnPoints;
}

TArray<FVector> UPW_SpawnPointsHandlerComponent::GetRandomCurrencySpawnPoints(int32 amount)
{
	TArray<FVector> avaliableSlots = _currencySpawnPoint.spawnPoints;
	TArray<FVector> result;
	if(amount > avaliableSlots.Num()) amount = avaliableSlots.Num();
	
	for (int32 i = 0; i < amount; i++)
	{
		int32 index = FMath::RandRange(0, avaliableSlots.Num() - 1);
		result.Add(ConvertToWorldLocation(avaliableSlots[index]));
		avaliableSlots.RemoveAt(index);
	}
	return result;
}

FVector UPW_SpawnPointsHandlerComponent::ConvertToWorldLocation(FVector location)
{
	return  GetComponentLocation() + GetComponentTransform().TransformVector(location);
}

