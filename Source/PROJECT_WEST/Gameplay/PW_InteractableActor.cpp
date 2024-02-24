// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_InteractableActor.h"

// Sets default values
APW_InteractableActor::APW_InteractableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APW_InteractableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APW_InteractableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}