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
	// DEBUG_STRING (FString::Printf (TEXT ("Num Players: %d"), numPlayers));

	APlayerState* playerState = NewPlayer->GetPlayerState<APlayerState>();
	if (playerState)
	{
		FString playerName = playerState->GetPlayerName();	
		DEBUG_STRING (FString::Printf (TEXT ("%s has joined session " ), *playerName));
	}
}

void APW_GameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	
	APlayerState* playerState = Exiting->GetPlayerState<APlayerState>();
	if (playerState)
	{
		FString playerName = playerState->GetPlayerName();
		DEBUG_STRING (FString::Printf (TEXT ("%s has left sesson " ), *playerName));
		
		APW_Character * character = Cast<APW_Character>(playerState->GetPawn());
		if (character)
		{
			DEBUG_STRING( "Player Logout Drop Item");
			character->Elim(true);
			character->ServerLeaveGame();
			character->Destroy();
		}
		else
		{
			DEBUG_STRING( "Player Logout No APW_Character");
		}
	}
	else
	{
		//DEBUG_STRING( "Player Logout No PlayerState");
	}

	int32 numPlayers = GameState.Get()->PlayerArray.Num();
	//DEBUG_STRING (FString::Printf (TEXT ("Num Players: %d"), numPlayers));
}

void APW_GameMode::ServerTravel(FString MapPath)
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->ServerTravel( MapPath + "?listen" );
	}
}

void APW_GameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APW_GameMode::PlayerEliminated(APW_Character* ElimmedCharacter, APW_PlayerController* VictimController,APlayerController* AttackerController)
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
