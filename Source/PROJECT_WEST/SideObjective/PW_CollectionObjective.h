// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW_SideObjective.h"
#include "PROJECT_WEST/PW_ItemObject.h"
#include "PW_CollectionObjective.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API APW_CollectionObjective : public APW_SideObjective
{
	GENERATED_BODY()


	virtual void SetUp(FSideObjectiveEntry sideObjectiveData, class APW_PoiArea* poiArea)override;
	virtual void Deactivate() override;

	void TryAssignOnCollectEvent(AActor* Actor);
	
	UFUNCTION()
	void OnTargetCollected(APW_ItemObject* ItemPicked);
	
};
