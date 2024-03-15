// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "PW_SpawnPointsManager.generated.h"


USTRUCT(BlueprintType)
struct FSpawnPoints
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "SpawnPoints", Meta = (MakeEditWidget = true))
	TArray<class APW_SpawnPoint*>  _spawnPoints;

	APW_SpawnPoint* GetRandomSpawnPoint()
	{
		APW_SpawnPoint* result = _spawnPoints.Num() > 0 ? _spawnPoints[FMath::RandRange(0, _spawnPoints.Num() - 1)] : nullptr;
		return result;
	}
};


UCLASS()
class PROJECT_WEST_API APW_SpawnPointsManager : public AActor
{
	GENERATED_BODY()
	
public:	

	APW_SpawnPointsManager();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "SpawnPoints", meta = (AllowPrivateAccess = "true") )
	class APW_SpawnPoint* _lanternSpawnPoint;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "SpawnPoints", meta = (AllowPrivateAccess = "true") )
	FSpawnPoints _currencySpawnPoints;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "SpawnPoints", meta = (AllowPrivateAccess = "true") )
	FSpawnPoints _bountyPortalSpawnPoint;
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "SpawnPoints", meta = (AllowPrivateAccess = "true") )
	FSpawnPoints _extractionPoint;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "SpawnPoints", meta = (AllowPrivateAccess = "true") )
	FSpawnPoints _enemySpawnPoints;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "SpawnPoints", meta = (AllowPrivateAccess = "true") )
	class APW_SpawnPoint* _debtCollectorSpawnPoint;

public:

	UFUNCTION(BlueprintCallable)
	FVector GetBountyPortalSpawnPoint();

	FVector GetExtractionSpawnPoint();

	TArray<FVector>  GetEnemySpawnPoint();

	TArray<FVector> GetRandomCurrencySpawnPoints(int32 amount) const;
	
	FVector GetLanternSpawnPoint() const;
	FVector GetDebtCollectorSpawnPoint() const;
};
