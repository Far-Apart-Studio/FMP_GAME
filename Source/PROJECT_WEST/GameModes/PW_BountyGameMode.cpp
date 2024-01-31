// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_BountyGameMode.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "PROJECT_WEST/GameState/PW_GameState.h"
#include "PROJECT_WEST/PlayerState/PW_PlayerState.h"
#include "PROJECT_WEST/PlayerController/PW_PlayerController.h"

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

void APW_BountyGameMode::EnemyEliminated(APW_CharacterController* AttackerCharacter, APW_PlayerController* AttackerController)
{
	APW_PlayerState* attackerState = AttackerController ? Cast<APW_PlayerState>(AttackerController->PlayerState) : nullptr;

	APW_GameState* gameState = GetGameState<APW_GameState>();
	if (gameState)
	{
		gameState->UpdateTopScore(AttackerController->GetPlayerState<APW_PlayerState>());
	}
}