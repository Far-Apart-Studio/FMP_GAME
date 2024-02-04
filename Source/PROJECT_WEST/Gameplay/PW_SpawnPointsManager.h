// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PW_SpawnPointsHandlerComponent.h"

#include "PW_SpawnPointsManager.generated.h"

UCLASS()
class PROJECT_WEST_API APW_SpawnPointsManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APW_SpawnPointsManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE class UPW_SpawnPointsHandlerComponent* GetSpawnPointsHandlerComponent() const { return _spawnPointsHandlerComponent; }

private:

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "SpawnPoints", meta = (AllowPrivateAccess = "true") )
	UPW_SpawnPointsHandlerComponent* _spawnPointsHandlerComponent;
};
