#pragma once

#include "CoreMinimal.h"
#include "SideObjective.generated.h"

UENUM(BlueprintType)
enum class ETaskType : uint8
{
	EElimination,
	EDemolition,
	ECollection
};

USTRUCT(BlueprintType)
struct FSideObjectiveData
{
	GENERATED_BODY()
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data)
	FString _objectiveName;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data)
	FString _objectiveDescription;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data)
	ETaskType _taskType;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data)
	FString _poiID;
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data)
	int32 _objectiveDuration;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data)
	int32 _objectiveReward;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data)
	TSubclassOf<AActor> _objectiveObjectType;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data)
	int32 _objectiveAmount;
};
