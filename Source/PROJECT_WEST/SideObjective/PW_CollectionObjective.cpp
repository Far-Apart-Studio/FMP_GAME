// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_CollectionObjective.h"

#include "PROJECT_WEST/PW_ItemObject.h"
#include "PROJECT_WEST/Gameplay/Spawner/PW_SpawnPoint.h"
#include "PROJECT_WEST/POI/PW_PoiArea.h"

void APW_CollectionObjective::SetUp(FSideObjectiveEntry sideObjectiveEntry, APW_PoiArea* poiArea)
{
	Super::SetUp(sideObjectiveEntry, poiArea);

	FSpawnPoints spawnPoints = poiArea->GetSpawnPoints();

	for (int i = 0; i < sideObjectiveEntry._sideObjectiveInfo._objectiveAmount; i++)
	{
		const APW_SpawnPoint* spawnPoint  = poiArea->GetSpawnPoints().GetRandomSpawnPoint();
		
		if(spawnPoints._spawnPoints.Num() != 0)
		{
			const int spawnPointIndex = FMath::RandRange(0, spawnPoints._spawnPoints.Num() - 1);
			spawnPoint = spawnPoints._spawnPoints[spawnPointIndex];
			spawnPoints._spawnPoints.RemoveAt(spawnPointIndex);
		}

		if (!spawnPoint)
			continue;
		
		AActor* targetActor = GetWorld()->SpawnActor<AActor>(sideObjectiveEntry._sideObjectiveInfo._objectiveObjectType, spawnPoint->GetPosition(), spawnPoint->GetActorRotation());
		
		TryAssignOnCollectEvent(targetActor);
		_targetActors.Add(targetActor);
	}
}

void APW_CollectionObjective::Failed()
{
	for (AActor* targetActor : _targetActors)
	{
		if(!IsValid(targetActor)) continue;
		if (APW_ItemObject* itemObject = Cast<APW_ItemObject>(targetActor))
		{
			itemObject->_onPickedUpServer.RemoveDynamic(this, &APW_CollectionObjective::OnTargetCollected);
		}
		targetActor->Destroy();
	}

	Super::Failed();
}

void APW_CollectionObjective::TryAssignOnCollectEvent(AActor* Actor)
{
	if (APW_ItemObject* itemObject = Cast<APW_ItemObject>(Actor))
	{
		itemObject->_onPickedUpServer.AddDynamic(this, &APW_CollectionObjective::OnTargetCollected);
	}
}

void APW_CollectionObjective::OnTargetCollected(APW_ItemObject* ItemPicked)
{
	if (_targetActors.Contains(ItemPicked))
	{
		ItemPicked->_onPickedUpServer.RemoveDynamic(this, &APW_CollectionObjective::OnTargetCollected);
		_targetActors.Remove(ItemPicked);
		_currentObjectiveAmount++;
		if (_targetActors.Num() == 0)
		{
			Completed();
		}
	}
}
