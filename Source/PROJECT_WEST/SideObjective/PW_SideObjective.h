// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PROJECT_WEST/SideObjective/SideObjective.h"
#include "PW_SideObjective.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveCompleted, APW_SideObjective*, complectedObjective);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveFailed, APW_SideObjective*, failedObjective);

UCLASS()
class PROJECT_WEST_API APW_SideObjective : public AActor
{
	GENERATED_BODY()
	
public:	

	APW_SideObjective();

	virtual void Tick(float DeltaTime) override;
	virtual void Activate(FSideObjectiveData sideObjectiveData);
	virtual void Deactivate();

protected:

	virtual void BeginPlay() override;

private:
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	FSideObjectiveData _objectiveData;
};
