// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_Currency.h"
#include "PROJECT_WEST/PlayerController/PW_PlayerController.h"

APW_Currency::APW_Currency()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = _mesh;
}

void APW_Currency::BeginPlay()
{
	Super::BeginPlay();
	SetReplicates(true);
	_currentValue = FMath::RandRange(_minValue, _maxValue);
}

void APW_Currency::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APW_Currency::StartFocus_Implementation(AActor* owner)
{
	_mesh->SetRenderCustomDepth(true);
}

void APW_Currency::EndFocus_Implementation(AActor* owner)
{
	_mesh->SetRenderCustomDepth(false);
}

void APW_Currency::StartInteract_Implementation(AActor* owner)
{
	APW_PlayerController* playerController = Cast<APW_PlayerController>(owner->GetInstigatorController());
	if (playerController)
	{
		playerController->CollectCurrency(this);
	}
}