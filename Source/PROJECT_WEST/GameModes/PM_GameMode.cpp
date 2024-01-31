// Fill out your copyright notice in the Description page of Project Settings.


#include "PM_GameMode.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include  "PROJECT_WEST/PlayerState/PW_PlayerState.h"
#include "PROJECT_WEST/PlayerController/PW_PlayerController.h"

APM_GameMode::APM_GameMode()
{
	bUseSeamlessTravel = true;
}

void APM_GameMode::PostLogin(APlayerController* NewPlayer)
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

void APM_GameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	
	APlayerState* playerState = Exiting->GetPlayerState<APlayerState>();
	if (playerState)
	{
		FString playerName = playerState->GetPlayerName();
		GEngine->AddOnScreenDebugMessage (-1, 5.f, FColor::Red, FString::Printf (TEXT ("%s has left" ), *playerName));
	}

	int32 numPlayers = GameState.Get()->PlayerArray.Num();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Num Players: %d"), numPlayers - 1));
}

void APM_GameMode::ServerTravel(FString MapPath)
{
}

void APM_GameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APM_GameMode::PlayerEliminated(APW_CharacterController* ElimmedCharacter, APW_PlayerController* VictimController,APlayerController* AttackerController)
{
	APW_PlayerState* victimState = VictimController ? Cast<APW_PlayerState>(VictimController->PlayerState) : nullptr;

	if (ElimmedCharacter)
	{
		// ElimmedCharacter->OnEliminated();
	}
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
