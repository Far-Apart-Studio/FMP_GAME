// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_CollectionObjective.h"

#include "PROJECT_WEST/PW_ItemObject.h"
#include "PROJECT_WEST/POI/PW_PoiArea.h"

void APW_CollectionObjective::SetUp(FSideObjectiveEntry sideObjectiveEntry, APW_PoiArea* poiArea)
{
	Super::SetUp(sideObjectiveEntry, poiArea);

	for (int i = 0; i < sideObjectiveEntry._sideObjectiveInfo._objectiveAmount; i++)
	{
		AActor* targetActor = poiArea->SpawnActor(sideObjectiveEntry._sideObjectiveInfo._objectiveObjectType);
		TryAssignOnCollectEvent(targetActor);
		_targetActors.Add(targetActor);
	}
}

void APW_CollectionObjective::Deactivate()
{
	for (AActor* targetActor : _targetActors)
	{
		if (APW_ItemObject* itemObject = Cast<APW_ItemObject>(targetActor))
		{
			itemObject->_onPickedUpServer.RemoveDynamic(this, &APW_CollectionObjective::OnTargetCollected);
		}
		targetActor->Destroy();
	}

	Super::Deactivate();
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
		if (_targetActors.Num() == 0)
		{
			_onObjectiveCompleted.Broadcast(this);
		}
	}
}
