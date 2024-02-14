// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PW_SpawnPointsHandlerComponent.generated.h"

USTRUCT(BlueprintType)
struct FSpawnPointSet
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "SpawnPoints", Meta = (MakeEditWidget = true))
	TArray<FVector>  spawnPoints;

	FVector GetRandomSpawnPoint()
	{
		if (spawnPoints.Num() > 0)
		{
			return spawnPoints[FMath::RandRange(0, spawnPoints.Num() - 1)];
		}
		else
		{
			return FVector::ZeroVector;
		}
	}
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_SpawnPointsHandlerComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPW_SpawnPointsHandlerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(EditAnywhere, Category = "SpawnPoints")
	FSpawnPointSet _lanternSpawnPoint;

	UPROPERTY(EditAnywhere, Category = "SpawnPoints")
	FSpawnPointSet _bountySpawnPoint;

	UPROPERTY(EditAnywhere, Category = "SpawnPoints")
	FSpawnPointSet _extractionPoint;

	UPROPERTY(EditAnywhere, Category = "SpawnPoints")
	FSpawnPointSet _enemySpawnPoints;

	UPROPERTY(EditAnywhere, Category = "SpawnPoints")
	FSpawnPointSet _debtCollectorSpawnPoint;

	FVector GetLanternSpawnPoint();

	FVector GetBountySpawnPoint();

	FVector GetExtractionSpawnPoint();

	TArray<FVector>  GetEnemySpawnPoint();

private:

	FVector ConvertToWorldLocation(FVector location);
};
