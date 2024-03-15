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

APW_PoiArea* APW_PoiManager::GetPOIWithID(FString poiID)
{
	if (_poiAvailable.Num() == 0)
	{
		return nullptr;
	}
	
	APW_PoiArea* poiArea = _poiAvailable[0];
	for (APW_PoiArea* area : _poiAvailable)
	{
		if (area->GetPoiID() == poiID)
		{
			poiArea = area;
			break;
		}
	}
	_poiAvailable.Remove(poiArea);
	return poiArea;
}

