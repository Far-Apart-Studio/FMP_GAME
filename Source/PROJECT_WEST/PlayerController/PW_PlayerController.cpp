// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_PlayerController.h"

void APW_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void APW_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APW_PlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

float APW_PlayerController::GetServerTime()
{
	return  0;
}

void APW_PlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
}

void APW_PlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void APW_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}
