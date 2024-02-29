#pragma once

#include "CoreMinimal.h"
#include "PW_WeightedSpawner.generated.h"

#pragma once

USTRUCT(BlueprintType)
struct FSpawnInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "SpawnInfo")
	TSubclassOf<AActor> _actorClass;
	
	UPROPERTY(EditAnywhere, Category = "SpawnInfo")
	int _amount;
};

USTRUCT(BlueprintType)
struct FSpawnActorEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "SpawnInfo" , meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float _weight;
	
	UPROPERTY(EditAnywhere, Category = "SpawnInfo")
	TArray<FSpawnInfo> _spawnInfoList;
};

USTRUCT(BlueprintType)
struct FWeightedSpawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "SpawnInfo")
	TArray<FSpawnActorEntry> _actors;

	TArray<FSpawnInfo> GetRandomActorClass()
	{
		TArray<FSpawnInfo> result;
		
		float totalWeight = 0.0f;
		for (const FSpawnActorEntry& entry : _actors)
		{
			totalWeight += entry._weight;
		}

		float random = FMath::FRandRange(0.0f, totalWeight);
		for (const FSpawnActorEntry& entry : _actors)
		{
			random -= entry._weight;
			if (random <= 0.0f)
			{
				result = entry._spawnInfoList;
				break;
			}
		}
		return result;
	}
};
