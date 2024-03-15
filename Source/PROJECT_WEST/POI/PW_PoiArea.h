// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PROJECT_WEST/Gameplay/Spawner/PW_WeightedSpawner.h"
#include "GameFramework/Actor.h"
#include "PW_PoiArea.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPoiTriggered, APW_PoiArea*, poi);

UCLASS()
class PROJECT_WEST_API APW_PoiArea : public AActor
{
	GENERATED_BODY()
	
public:
	
	APW_PoiArea();

private:

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	FString _poiID;

	UPROPERTY (VisibleAnywhere , meta = (AllowPrivateAccess = "true"))
	class USceneComponent* _root;

	UPROPERTY(EditAnywhere, Category = "Gameplay",meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* _spawnArea;

	UPROPERTY(EditAnywhere, Category = "Gameplay",meta = (AllowPrivateAccess = "true"))
	class USphereComponent* _detectionTrigger;

	UPROPERTY (VisibleAnywhere , Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> _spawnedEnemies;

	UPROPERTY(EditAnywhere, Category = "Gameplay",meta = (AllowPrivateAccess = "true"))
	FWeightedSpawn _enemyWeightedSpawn;
	
	UPROPERTY (VisibleAnywhere , Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> _triggeredActors;

	UPROPERTY (VisibleAnywhere , Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	class UPW_BoxSpawningComponent* _boxSpawningComponent;

	bool _isTriggered = false;
	
private:

	void SpawnEnemies();
	bool CanSpawnEnemy();
	int GetNumberOfCharactersInLevel();
	
	void TryAssignDeathEvent(AActor* actor);
	void TryFadeActorMaterial(AActor* actor);
	void TryAssignUnloaderEvent(AActor* actor);

	UFUNCTION()
	void OnEnemyDeath(AActor* OwnerActor,AActor* DamageCauser, AController* DamageCauserController);

	UFUNCTION()
	void OnActorUnloaded(AActor* UnloadedActor);
	
protected:

	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnDetectionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDetectionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
public:	

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "POI")
	FString GetPoiID() const { return _poiID; }

	UFUNCTION(BlueprintCallable)
	AActor* SpawnPOIEnemy(TSubclassOf<AActor> actorClass);

	UFUNCTION(BlueprintCallable)
	AActor* SpawnActor(TSubclassOf<AActor> actorClass);

	UPROPERTY(BlueprintAssignable, Category = "POI")
	FOnPoiTriggered OnPoiTriggered;
};
