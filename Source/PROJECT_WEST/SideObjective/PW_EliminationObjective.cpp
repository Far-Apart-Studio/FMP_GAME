// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_EliminationObjective.h"

#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/PW_HealthComponent.h"
#include "PROJECT_WEST/Gameplay/Components/PW_DistanceUnloaderComponent.h"
#include "PROJECT_WEST/Gameplay/Components/PW_MaterialEffectComponent.h"
#include "PROJECT_WEST/Gameplay/Spawner/PW_SpawnPoint.h"
#include "PROJECT_WEST/POI/PW_PoiArea.h"

void APW_EliminationObjective::SetUp(FSideObjectiveEntry sideObjectiveEntry, APW_PoiArea* poiArea)
{
	Super::SetUp(sideObjectiveEntry, poiArea);
}

void APW_EliminationObjective::Failed()
{
	for (AActor* targetActor : _targetActors)
	{
		if(!IsValid(targetActor)) continue;
		if (UPW_HealthComponent* healthComponent = targetActor->FindComponentByClass<UPW_HealthComponent>())
		{
			healthComponent->OnDeathServer.RemoveDynamic(this, &APW_EliminationObjective::OnTargetDeath);
		}
		targetActor->Destroy();
	}
	
	Super::Failed();
}

void APW_EliminationObjective::OnPOITriggered(APW_PoiArea* poiArea)
{
	Super::OnPOITriggered(poiArea);

	FSpawnPoints spawnPoints = poiArea->GetSpawnPoints();
	
	for (int i = 0; i < _objectiveData._sideObjectiveInfo._objectiveAmount; i++)
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

		DEBUG_STRING ("Spawned actor: " + _objectiveData._sideObjectiveInfo._objectiveObjectType->GetName());

		AActor* targetActor = GetWorld()->SpawnActor<AActor>(_objectiveData._sideObjectiveInfo._objectiveObjectType, spawnPoint->GetPosition(), spawnPoint->GetActorRotation());
		TryFadeActorMaterial(targetActor);
		TryDisableDistanceUnloader(targetActor);
		TryAssignDeathEvent(targetActor);
		_targetActors.Add(targetActor);
	}
}

void APW_EliminationObjective::TryFadeActorMaterial(AActor* actor)
{
	UPW_MaterialEffectComponent* materialEffectComponent = actor->FindComponentByClass<UPW_MaterialEffectComponent>();
	if (materialEffectComponent)
	{
		materialEffectComponent->ActivateEffect(EEffectDirection::ED_Backward);
	}
}

void APW_EliminationObjective::TryDisableDistanceUnloader(AActor* actor)
{
	UPW_DistanceUnloaderComponent* distanceUnloaderComponent = actor->FindComponentByClass<UPW_DistanceUnloaderComponent>();
	if (distanceUnloaderComponent)
	{
		distanceUnloaderComponent->SetCanUnload (false);
	}
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
			Completed();
		}
	}
}
