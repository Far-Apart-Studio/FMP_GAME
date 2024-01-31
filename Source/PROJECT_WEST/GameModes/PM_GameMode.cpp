// Fill out your copyright notice in the Description page of Project Settings.


#include "PM_GameMode.h"

APM_GameMode::APM_GameMode()
{
}

void APM_GameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APM_GameMode::PlayerEliminated(APW_CharacterController* ElimmedCharacter, APW_PlayerController* VictimController,APlayerController* AttackerController)
{
}

void APM_GameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
}

void APM_GameMode::PlayerLeftGame(APW_PlayerState* PlayerLeaving)
{
}

float APM_GameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	return 0;
}

void APM_GameMode::BeginPlay()
{
	Super::BeginPlay();
}

void APM_GameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();
}
