// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_GameMode.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include  "PROJECT_WEST/PlayerState/PW_PlayerState.h"
#include "PROJECT_WEST/PlayerController/PW_PlayerController.h"
#include "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/DebugMacros.h"

APW_GameMode::APW_GameMode()
{
	bUseSeamlessTravel = true;
}

void APW_GameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if(!GameState) return;
	
	int32 numPlayers = GameState.Get()->PlayerArray.Num();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Num Players: %d"), numPlayers));

	APlayerState* playerState = NewPlayer->GetPlayerState<APlayerState>();
	if (playerState)
	{
		FString playerName = playerState->GetPlayerName();	
		GEngine->AddOnScreenDebugMessage (-1, 5.f, FColor::Red, FString::Printf (TEXT ("%s has joined" ), *playerName));
	}
}

void APW_GameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	
	APlayerState* playerState = Exiting->GetPlayerState<APlayerState>();
	if (playerState)
	{
		FString playerName = playerState->GetPlayerName();
		GEngine->AddOnScreenDebugMessage (-1, 5.f, FColor::Red, FString::Printf (TEXT ("%s has left" ), *playerName));
	}

	// TODO: Drop item if player is holding one
	APW_Character * character = Exiting ? Cast<APW_Character>(Exiting->GetPawn()) : nullptr;
	if (character)
	{
		DEBUG_STRING( "Player Logout Drop Item");
		character->Elim(true);
		character->ServerLeaveGame();
		character->Destroy();
	}

	int32 numPlayers = GameState.Get()->PlayerArray.Num();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Num Players: %d"), numPlayers - 1));
}

void APW_GameMode::ServerTravel(FString MapPath)
{
}

void APW_GameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APW_GameMode::PlayerEliminated(APW_CharacterController* ElimmedCharacter, APW_PlayerController* VictimController,APlayerController* AttackerController)
{
	APW_PlayerState* victimState = VictimController ? Cast<APW_PlayerState>(VictimController->PlayerState) : nullptr;

	if (ElimmedCharacter)
	{
		// ElimmedCharacter->OnEliminated();
	}
}

void APW_GameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
}

void APW_GameMode::PlayerLeftGame(APW_PlayerState* PlayerLeaving)
{
	if (!PlayerLeaving) return;
	
	APW_Character * character = PlayerLeaving ? Cast<APW_Character>(PlayerLeaving->GetPawn()) : nullptr;
	if (character)
	{
		DEBUG_STRING( "Player left game Drop Item");
		 character->Elim(true);
	}
}

float APW_GameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	return 0;
}

void APW_GameMode::BeginPlay()
{
	Super::BeginPlay();
}

void APW_GameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();
}
