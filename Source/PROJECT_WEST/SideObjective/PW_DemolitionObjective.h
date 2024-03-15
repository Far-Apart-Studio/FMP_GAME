// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW_SideObjective.h"
#include "PW_DemolitionObjective.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API APW_DemolitionObjective : public APW_SideObjective
{
	GENERATED_BODY()

private:

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	TArray< AActor*> _targetActors;
	
public:
	
	virtual void SetUp(FSideObjectiveEntry sideObjectiveData, class APW_PoiArea* poiArea)override;
	virtual void Deactivate() override;
	
	void TryAssignDeathEvent(AActor* actor);

	UFUNCTION()
	void OnTargetDeath(AActor* OwnerActor,AActor* DamageCauser, AController* DamageCauserController);
};
