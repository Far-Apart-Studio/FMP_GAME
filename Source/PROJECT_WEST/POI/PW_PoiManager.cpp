// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_PoiManager.h"
#include "PW_PoiArea.h"
#include "EngineUtils.h"
#include "PROJECT_WEST/DebugMacros.h"

APW_PoiManager::APW_PoiManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APW_PoiManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	GetAllChildPoIActors();
}

void APW_PoiManager::BeginPlay()
{
	Super::BeginPlay();
	_poiAvailable = _poiAreas;
}

void APW_PoiManager::GetAllChildPoIActors()
{
	TArray<APW_PoiArea*> poiAreas = TArray<APW_PoiArea*>();
	for (TActorIterator<APW_PoiArea> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		APW_PoiArea* poiArea = *ActorItr;
		poiAreas.Add(poiArea);
	}
	_poiAreas = poiAreas;
}

void APW_PoiManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

APW_PoiArea* APW_PoiManager::GetRandomPOI()
{
	if (_poiAvailable.Num() == 0)
	{
		return nullptr;
	}
	const int32 randomIndex = FMath::RandRange(0, _poiAvailable.Num() - 1);
	APW_PoiArea* poiArea = _poiAvailable[randomIndex];
	_poiAvailable.RemoveAt(randomIndex);
	return poiArea;
}

