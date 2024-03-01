// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW_WeightedSpawner.h"
#include "GameFramework/Actor.h"
#include "PW_AutoEnemySpawner.generated.h"

UCLASS()
class PROJECT_WEST_API APW_AutoEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APW_AutoEnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetCharacter(ACharacter* character);

private:
	
	UPROPERTY(VisibleAnywhere, Category = "Gameplay",meta = (AllowPrivateAccess = "true"))
	class ACharacter* _character;

	UPROPERTY (VisibleAnywhere , Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	class UPW_BoxSpawningComponent* _boxSpawningComponent;

	UPROPERTY (VisibleAnywhere , Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> _spawnedActors;

	UPROPERTY (VisibleAnywhere , meta = (AllowPrivateAccess = "true"))
	class USceneComponent* _root;

	UPROPERTY(VisibleAnywhere, Category = "Gameplay",meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* _spawnArea;
	
	UPROPERTY(EditAnywhere, Category = "Gameplay",meta = (AllowPrivateAccess = "true"))
	FWeightedSpawn _weightedSpawn;

	UPROPERTY(EditAnywhere, Category = "Gameplay",meta = (AllowPrivateAccess = "true"))
	float _distanceFromPlayer;

	UPROPERTY(EditAnywhere, Category = "Gameplay",meta = (AllowPrivateAccess = "true"))
	float _spawnRate;

	UPROPERTY(EditAnywhere, Category = "Gameplay",meta = (AllowPrivateAccess = "true"))
	int32 _maxSpawnedEnemies;

	UPROPERTY(EditAnywhere, Category = "Gameplay",meta = (AllowPrivateAccess = "true"))
	int32 _minDistanceToPlayer;

	float _spawnTimer;

	float _whileLoopBreakerMaxCount;

	class UPW_HealthComponent* _healthComponent;

private:

	void HandleSpawnTimer(float DeltaTime);
	void HandlePositioning();
	
	void SpawnEnemies();
	bool GetValidSpawnPosition(FVector& outPosition);
	bool AnyPlayerInRange(FVector location);

	void TryAssignDeathEvent(AActor* actor);
	void TryFadeActorMaterial(AActor* actor);
	void TryAssignUnloaderEvent(AActor* actor);
	

	UFUNCTION()
	void OnActorDeath(AActor* OwnerActor,AActor* DamageCauser, AController* DamageCauserController);

	UFUNCTION()
	void OnActorUnloaded(AActor* UnloadedActor);
};
