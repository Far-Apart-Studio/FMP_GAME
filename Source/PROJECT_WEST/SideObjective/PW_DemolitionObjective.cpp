// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_DemolitionObjective.h"

#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/PW_HealthComponent.h"
#include "PROJECT_WEST/POI/PW_PoiArea.h"

void APW_DemolitionObjective::SetUp(FSideObjectiveData sideObjectiveData, APW_PoiArea* poiArea)
{
	Super::SetUp(sideObjectiveData, poiArea);
	_targetActor = poiArea->SpawnActor(sideObjectiveData._objectiveObjectType);
	TryAssignDeathEvent(_targetActor);
}

void APW_DemolitionObjective::Deactivate()
{
	Super::Deactivate();
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
	_onObjectiveCompleted.Broadcast(this);
}
