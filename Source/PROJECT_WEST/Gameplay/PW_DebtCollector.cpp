// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_DebtCollector.h"

// Sets default values
APW_DebtCollector::APW_DebtCollector()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APW_DebtCollector::BeginPlay()
{
	Super::BeginPlay();
	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called every frame
void APW_DebtCollector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APW_DebtCollector::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

