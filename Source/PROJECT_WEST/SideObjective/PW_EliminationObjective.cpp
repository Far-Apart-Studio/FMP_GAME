// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_EliminationObjective.h"

#include "PROJECT_WEST/PW_HealthComponent.h"
#include "PROJECT_WEST/POI/PW_PoiArea.h"

void APW_EliminationObjective::SetUp(FSideObjectiveEntry sideObjectiveEntry, APW_PoiArea* poiArea)
{
	Super::SetUp(sideObjectiveEntry, poiArea);
	for (int i = 0; i < sideObjectiveEntry._sideObjectiveInfo._objectiveAmount; i++)
	{
		AActor* targetActor = poiArea->SpawnActor(sideObjectiveEntry._sideObjectiveInfo._objectiveObjectType);
		TryAssignDeathEvent(targetActor);
		_targetActors.Add(targetActor);
	}
}

void APW_EliminationObjective::Deactivate()
{
	for (AActor* targetActor : _targetActors)
	{
		if (UPW_HealthComponent* healthComponent = targetActor->FindComponentByClass<UPW_HealthComponent>())
		{
			healthComponent->OnDeathServer.RemoveDynamic(this, &APW_EliminationObjective::OnTargetDeath);
		}
		targetActor->Destroy();
	}
	
	Super::Deactivate();
}

void APW_EliminationObjective::TryAssignDeathEvent(AActor* Actor)
{
	UPW_HealthComponent* healthComponent = Actor->FindComponentByClass<UPW_HealthComponent>();
	if (healthComponent)
	{
		healthComponent->OnDeathServer.AddDynamic(this, &APW_EliminationObjective::OnTargetDeath);
	}
}

void APW_EliminationObjective::OnTargetDeath(AActor* OwnerActor, AActor* DamageCauser,
	AController* DamageCauserController)
{
	if (_targetActors.Contains(OwnerActor))
	{
		_targetActors.Remove(OwnerActor);
		_currentObjectiveAmount++;
		if (_targetActors.Num() == 0)
		{
			_onObjectiveCompleted.Broadcast(this);
			_completed = true;
		}
	}
}
