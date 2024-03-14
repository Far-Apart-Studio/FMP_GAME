// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_PoiManager.h"
#include "PW_PoiArea.h"
#include "EngineUtils.h"

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
	
}

void APW_PoiManager::GetAllChildPoIActors()
{
	TArray<AActor*> childActors;
	GetAllChildActors(childActors);
	TArray<APW_PoiArea*> poiAreas;
	for (AActor* actor : childActors)
	{
		APW_PoiArea* poiArea = Cast<APW_PoiArea>(actor);
		if (poiArea)
		{
			poiAreas.Add(poiArea);
		}
	}
	_poiAreas = poiAreas;
}

void APW_PoiManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

