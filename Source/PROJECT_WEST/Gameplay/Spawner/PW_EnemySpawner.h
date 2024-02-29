// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW_WeightedSpawner.h"
#include "GameFramework/Actor.h"
#include "PW_EnemySpawner.generated.h"

UCLASS()
class PROJECT_WEST_API APW_EnemySpawner : public AActor
{
	GENERATED_BODY()

private:

	UPROPERTY (VisibleAnywhere , meta = (AllowPrivateAccess = "true"))
	class USceneComponent* _root;

	UPROPERTY(EditAnywhere, Category = "Gameplay",meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* _spawnArea;

	UPROPERTY(EditAnywhere, Category = "Gameplay",meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* _detectionBox;

	UPROPERTY (VisibleAnywhere , Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> _spawnedActors;

	UPROPERTY(EditAnywhere, Category = "Gameplay",meta = (AllowPrivateAccess = "true"))
	FWeightedSpawn _weightedSpawn;

	UPROPERTY(EditAnywhere, Category = "Gameplay" , meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float _spawnChance;

	UPROPERTY(EditAnywhere, Category = "Gameplay" , meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float _spawnCooldown;

	UPROPERTY (VisibleAnywhere , Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> _triggredActors;

	UPROPERTY (VisibleAnywhere , Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	class UPW_BoxSpawningComponent* _boxSpawningComponent;
	
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

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	AActor* SpawnActor(TSubclassOf<AActor> actorClass);

private:

	void SpawnEnemies();

	bool CanSpawnEnemy();
	void TryAssignDeathEvent(AActor* actor);
	void TryFadeActorMaterial(AActor* actor);
	void TryAssignUnloaderEvent(AActor* actor);

	UFUNCTION()
	void OnActorDeath(AActor* OwnerActor,AActor* DamageCauser, AController* DamageCauserController);

	UFUNCTION()
	void OnActorUnloaded(AActor* UnloadedActor);
};
