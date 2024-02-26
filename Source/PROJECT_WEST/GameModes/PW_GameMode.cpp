// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_GameMode.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include  "PROJECT_WEST/PlayerState/PW_PlayerState.h"
#include "PROJECT_WEST/PlayerController/PW_PlayerController.h"
#include "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/PW_MultiplayerSessionsSubsystem.h"
#include "PROJECT_WEST/Gameplay/PW_GameInstance.h"
#include "PROJECT_WEST/Items/PW_Currency.h"

APW_GameMode::APW_GameMode()
{
	bUseSeamlessTravel = true;
}

void APW_GameMode::BeginPlay()
{
	Super::BeginPlay();
	LoadGameSessionData();
}

void APW_GameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	APlayerState* playerState = NewPlayer->GetPlayerState<APlayerState>();
	if (playerState)
	{
		FString playerName = playerState->GetPlayerName();
		APW_PlayerController* playerController = Cast<APW_PlayerController>(NewPlayer);
		if (playerController)
		{
			playerController->SetNewPlayerName(playerName);
		}
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
	}
}

void APW_GameMode::ServerTravel(FString MapPath)
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->ServerTravel( MapPath + "?listen" );
	}
}

int32 APW_GameMode::GetNumPlayerInSession() const
{
	return GameState.Get()->PlayerArray.Num();
}

void APW_GameMode::AddMoney(int32 amount)
{
	if (_gameInstance)
	{
		_gameInstance->GetGameSessionData()._money += amount;
		NotifyPlayersOfMoney();
	}
}

void APW_GameMode::RemoveMoney(int32 amount)
{
	if (_gameInstance)
	{
		_gameInstance->GetGameSessionData()._money -= amount;
		NotifyPlayersOfMoney();
	}
}

void APW_GameMode::NotifyPlayersOfMoney()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APW_PlayerController* playerController = Cast<APW_PlayerController>(It->Get());
		if (playerController)
		{
			playerController->ClientMoneyValueChanged(_gameInstance->GetGameSessionData()._money);
		}
	}
}

void APW_GameMode::CollectCurrency(APW_Currency* Currency)
{
	AddMoney(Currency->GetCurrentValue());
	Currency->Destroy();
}

int32 APW_GameMode::GetMoney() const
{
	return _gameInstance ? _gameInstance->GetGameSessionData()._money : 0;
}

void APW_GameMode::SetDay(int day)
{
	if (_gameInstance)
	{
		_gameInstance->GetGameSessionData()._dayIndex = day;
		NofigyPlayersOfDay();
	}
}

void APW_GameMode::NofigyPlayersOfDay()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APW_PlayerController* playerController = Cast<APW_PlayerController>(It->Get());
		if (playerController)
		{
			playerController->ClientDayChanged(_gameInstance->GetGameSessionData()._dayIndex);
		}
	}
}

void APW_GameMode::TriggerPlayersAnnouncement(const FString& announcement, FColor color, float duration)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APW_PlayerController* playerController = Cast<APW_PlayerController>(It->Get());
		if (playerController)
		{
			playerController->ClientShowAnnocement( announcement, color, duration);
		}
	}
}

FString APW_GameMode::GetPlayerName(APlayerController* playerController) const
{
	FString playerName = "";
	if (playerController)
	{
		APlayerState* playerState = playerController->GetPlayerState<APlayerState>();
		if (playerState)
		{
			playerName = playerState->GetPlayerName();
		}
	}
	return playerName;
}

void APW_GameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APW_GameMode::LoadGameSessionData()
{
	_gameInstance = Cast<UPW_GameInstance>(GetGameInstance());
	if (_gameInstance)
	{
		NotifyPlayersOfMoney();
		//DEBUG_STRING("LoadGameSessionData Found");
	}
}

void APW_GameMode::PlayerEliminated(APW_Character* ElimmedCharacter, APW_PlayerController* VictimController,AController* AttackerController)
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

void APW_GameMode::ToggleSessionLock(bool lock)
{
	UPW_MultiplayerSessionsSubsystem* multiplayerSessionsSubsystem =  Cast<UPW_MultiplayerSessionsSubsystem>(GetGameInstance()->GetSubsystem<UPW_MultiplayerSessionsSubsystem>());
	if ( multiplayerSessionsSubsystem )
	{
		//DEBUG_STRING("MultiplayerSessionsSubsystem Found");
		multiplayerSessionsSubsystem->ToggleSessionStatus(lock);
	}
}

void APW_GameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();
}

void APW_GameMode::ToggleAllPlayersInput(bool bEnable)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APW_PlayerController* playerController = Cast<APW_PlayerController>(It->Get());
		if (playerController)
		{
			playerController->ClientTogglePlayerInput( bEnable );
		}
	}
}
