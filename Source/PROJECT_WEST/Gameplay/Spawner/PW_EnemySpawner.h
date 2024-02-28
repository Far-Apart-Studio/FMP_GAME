// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PW_EnemySpawner.generated.h"

USTRUCT(BlueprintType)
struct FEnemySpawnInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "SpawnItemInfo")
	TSubclassOf<AActor> _actorClass;
	
	UPROPERTY(EditAnywhere, Category = "SpawnItemInfo")
	int _amount;
};


USTRUCT(BlueprintType)
struct FSpawnActorEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "SpawnItemInfo" , meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float _weight;
	
	UPROPERTY(EditAnywhere, Category = "SpawnItemInfo")
	TArray<FEnemySpawnInfo> _enemySpawnInfoList;
};

USTRUCT(BlueprintType)
struct FSpawnActorInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "SpawnItemInfo")
	TArray<FSpawnActorEntry> _actors;

	TArray<FEnemySpawnInfo> GetRandomActorClass()
	{
		TArray<FEnemySpawnInfo> result;
		
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
				result = entry._enemySpawnInfoList;
				break;
			}
		}
		return result;
	}
};

UCLASS()
class PROJECT_WEST_API APW_EnemySpawner : public AActor
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, Category = "Gameplay",meta = (AllowPrivateAccess = "true"))
	FSpawnActorInfo _actorsToSpawn;

	UPROPERTY (VisibleAnywhere , meta = (AllowPrivateAccess = "true"))
	class USceneComponent* _root;

	UPROPERTY(EditAnywhere, Category = "Gameplay",meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* _spawnArea;

	UPROPERTY(EditAnywhere, Category = "Gameplay",meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* _detectionBox;

	UPROPERTY (VisibleAnywhere , Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> _spawnedActors;

	UPROPERTY(EditAnywhere, Category = "Gameplay",meta = (AllowPrivateAccess = "true"))
	float _spawnHeightOffset;

	UPROPERTY(EditAnywhere, Category = "Gameplay" , meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float _spawnChance;

	UPROPERTY(EditAnywhere, Category = "Gameplay" , meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float _spawnCooldown;

	UPROPERTY (VisibleAnywhere , Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> _triggredActors;
	
public:	

	APW_EnemySpawner();

protected:

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	
	UFUNCTION()
	void OnDetectionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDetectionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	AActor* SpawnActorInBox(TSubclassOf<AActor> actorClass);

private:

	void SpawnEnemies();

	bool CanSpawnEnemy();
	void TryAssignDeathEvent(AActor* actor);
	void TryFadeActorMaterial(AActor* actor);

	UFUNCTION()
	void OnActorDeath(AActor* OwnerActor,AActor* DamageCauser, AController* DamageCauserController);
	
	FVector GetRandomPositionInSpawnArea();
	bool IsPositionValid(FVector position);
	void GetGroundPositionAndNormal(FVector origin, FVector& outPosition, FVector& outNormal);
};
