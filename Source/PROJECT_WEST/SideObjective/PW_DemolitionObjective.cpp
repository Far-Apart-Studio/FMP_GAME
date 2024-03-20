// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_DemolitionObjective.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/PW_HealthComponent.h"
#include "PROJECT_WEST/Gameplay/Spawner/PW_SpawnPoint.h"
#include "PROJECT_WEST/POI/PW_PoiArea.h"

void APW_DemolitionObjective::SetUp(FSideObjectiveEntry sideObjectiveEntry, APW_PoiArea* poiArea)
{
	Super::SetUp(sideObjectiveEntry, poiArea);

	FSpawnPoints spawnPoints = poiArea->GetSpawnPoints();

	for (int i = 0; i < sideObjectiveEntry._sideObjectiveInfo._objectiveAmount; i++)
	{
		APW_SpawnPoint* spawnPoint  = poiArea->GetSpawnPoints().GetRandomSpawnPoint();
		
		if(spawnPoints._spawnPoints.Num() != 0)
		{
			const int spawnPointIndex = FMath::RandRange(0, spawnPoints._spawnPoints.Num() - 1);
			spawnPoint = spawnPoints._spawnPoints[spawnPointIndex];
			spawnPoints._spawnPoints.RemoveAt(spawnPointIndex);
		}

		if (!spawnPoint)
			continue;
		
		AActor* targetActor = GetWorld()->SpawnActor<AActor>(sideObjectiveEntry._sideObjectiveInfo._objectiveObjectType, spawnPoint->GetPosition(), spawnPoint->GetActorRotation());
		TryAssignDeathEvent(targetActor);
		_targetActors.Add(targetActor);
	}
}

void APW_DemolitionObjective::Failed()
{
	for (AActor* targetActor : _targetActors)
	{
		if (UPW_HealthComponent* healthComponent = targetActor->FindComponentByClass<UPW_HealthComponent>())
		{
			healthComponent->OnDeathServer.RemoveDynamic(this, &APW_DemolitionObjective::OnTargetDeath);
		}
		targetActor->Destroy();
	}
	
	Super::Failed();
}

void APW_DemolitionObjective::TryAssignDeathEvent(AActor* actor)
{
	if (UPW_HealthComponent* healthComponent = actor->FindComponentByClass<UPW_HealthComponent>())
	{
		healthComponent->OnDeathServer.AddDynamic(this, &APW_DemolitionObjective::OnTargetDeath);
	}
}

void APW_DemolitionObjective::OnTargetDeath(AActor* OwnerActor,AActor* DamageCauser, AController* DamageCauserController)
{
	_targetActors.Remove(OwnerActor);
	_currentObjectiveAmount++;
	if (_targetActors.Num() == 0)
	{
		Completed();
	}
}
