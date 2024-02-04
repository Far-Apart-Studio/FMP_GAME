// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_SpawnPointsManager.h"

// Sets default values
APW_SpawnPointsManager::APW_SpawnPointsManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_spawnPointsHandlerComponent = CreateDefaultSubobject<UPW_SpawnPointsHandlerComponent>(TEXT("SpawnPointsHandlerComponent"));
}

// Called when the game starts or when spawned
void APW_SpawnPointsManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APW_SpawnPointsManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

