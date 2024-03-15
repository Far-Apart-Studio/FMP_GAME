// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_SpawnPointsManager.h"

#include "PW_SpawnPoint.h"

// Sets default values
APW_SpawnPointsManager::APW_SpawnPointsManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APW_SpawnPointsManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APW_SpawnPointsManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector APW_SpawnPointsManager::GetBountyPortalSpawnPoint()
{
	APW_SpawnPoint* randomSpawnPoint = _bountyPortalSpawnPoint.GetRandomSpawnPoint();
	if (randomSpawnPoint)
	{
		return randomSpawnPoint->GetActorLocation();
	}
	else
	{
		return FVector::ZeroVector;
	}
}

FVector APW_SpawnPointsManager::GetExtractionSpawnPoint()
{
	APW_SpawnPoint* randomSpawnPoint = _extractionPoint.GetRandomSpawnPoint();
	if (randomSpawnPoint)
	{
		return randomSpawnPoint->GetActorLocation();
	}
	else
	{
		return FVector::ZeroVector;
	}
}

TArray<FVector> APW_SpawnPointsManager::GetEnemySpawnPoint()
{
	TArray<FVector> result;
	for (APW_SpawnPoint* spawnPoint : _enemySpawnPoints._spawnPoints)
	{
		result.Add(spawnPoint->GetActorLocation());
	}
	return result;
}

TArray<FVector> APW_SpawnPointsManager::GetRandomCurrencySpawnPoints(int32 amount) const
{
	TArray<	APW_SpawnPoint*> avaliableSlots = _currencySpawnPoints._spawnPoints;
	TArray<FVector> result;
	if (amount > avaliableSlots.Num()) amount = avaliableSlots.Num();

	for (int32 i = 0; i < amount; i++)
	{
		int32 index = FMath::RandRange(0, avaliableSlots.Num() - 1);
		result.Add(avaliableSlots[index]->GetActorLocation());
		avaliableSlots.RemoveAt(index);
	}
	return result;
}

FVector APW_SpawnPointsManager::GetLanternSpawnPoint() const
{
	return  _lanternSpawnPoint->GetPosition();
}

FVector APW_SpawnPointsManager::GetDebtCollectorSpawnPoint() const
{
	return _debtCollectorSpawnPoint->GetPosition();
}

