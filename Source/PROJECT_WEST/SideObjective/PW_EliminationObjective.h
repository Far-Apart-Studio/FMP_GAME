// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW_SideObjective.h"
#include "PW_EliminationObjective.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API APW_EliminationObjective : public APW_SideObjective
{
	GENERATED_BODY()

	virtual void SetUp(FSideObjectiveEntry sideObjectiveData, class APW_PoiArea* poiArea)override;
	virtual void Failed() override;

	void TryFadeActorMaterial(AActor* actor);
	void TryAssignDeathEvent(AActor* Actor);

	UFUNCTION()
	void OnTargetDeath(AActor* OwnerActor, AActor* DamageCauser, AController* DamageCauserController);
};
