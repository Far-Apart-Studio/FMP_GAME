// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_BountyGameMode.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"

APW_BountyGameMode::APW_BountyGameMode()
{
	bUseSeamlessTravel = true;
	_mapPath = "";

	//bDelayedStart = true;
}

void APW_BountyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void APW_BountyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}
