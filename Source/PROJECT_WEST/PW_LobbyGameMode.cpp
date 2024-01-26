// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_LobbyGameMode.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"

APW_LobbyGameMode::APW_LobbyGameMode()
{
	bUseSeamlessTravel = true;
	_playersNeeded = 2;
	_mapPath = "";
}

void APW_LobbyGameMode::PostLogin(APlayerController* NewPlayer)
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

	if (numPlayers >= _playersNeeded)
	{
		ServerTravel();
	}
	else
	{

	}
}

void APW_LobbyGameMode::Logout(AController* Exiting)
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

void APW_LobbyGameMode::ServerTravel()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->ServerTravel( _mapPath + "?listen" );
	}
}
