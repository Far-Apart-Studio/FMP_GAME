// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_GameMode.h"

#include "EngineUtils.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include  "PROJECT_WEST/PlayerState/PW_PlayerState.h"
#include "PROJECT_WEST/PlayerController/PW_PlayerController.h"
#include "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/PW_MultiplayerSessionsSubsystem.h"
#include "PROJECT_WEST/Gameplay/PW_GameInstance.h"
#include "PROJECT_WEST/Items/PW_Currency.h"
#include "PROJECT_WEST/PW_InventoryHandler.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

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
	
	if (APW_PlayerState* playerState = NewPlayer->GetPlayerState<APW_PlayerState>())
	{
		const int32 random = GetRandomColorIndex();
		//const int32 random = FMath::RandRange(0, _colorIndexes.Num() - 1);
		
		_gameInstance = Cast<UPW_GameInstance>(GetGameInstance());
		if (_gameInstance)
		{
			_gameInstance->GetGameSessionData()._playersVisualData.AddData(playerState->GetPlayerName(),random);
		}
		
		playerState->SetColorIndex(random);
		
		if (APW_PlayerController* playerController = Cast<APW_PlayerController>(NewPlayer))
		{
			DEBUG_STRING (FString::Printf (TEXT ("%s has joined session " ), *playerState->GetPlayerName()));
			playerController->ClientOnLoadedInGameMode();
			//playerController->SetNewPlayerName(playerName);
		}

		FNotificationEntry notification;
		notification._notificationType = ENotificationType::EInfo;
		notification._playerNameText = playerState->GetPlayerName();
		notification._notificationText = "Has joined the game";
		TriggerNotification(notification);
	}
}

void APW_GameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	
	APlayerState* playerState = Exiting->GetPlayerState<APlayerState>();
	if (playerState)
	{
		FString playerName = playerState->GetPlayerName();

		FNotificationEntry notification;
		notification._notificationType = ENotificationType::EInfo;
		notification._playerNameText = playerName;
		notification._notificationText = "Has left the game";
		TriggerNotification(notification);
		//DEBUG_STRING (FString::Printf (TEXT ("%s has left sesson " ), *playerName));
	}
}

void APW_GameMode::ServerTravel(FString MapPath)
{
	if (UWorld* World = GetWorld())
	{
		World->ServerTravel( MapPath + "?listen" );
	}
}

void APW_GameMode::ServerTravelMapEnty(FMapDataEntry mapdata)
{
	TriggerPlayersLoadingScreen(mapdata);
	ServerTravel (mapdata._mapPath);
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

void APW_GameMode::SaveAllPlayersInventoryData()
{
	_gameInstance->GetGameSessionData()._playersInventoryData.Reset();
	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APW_PlayerController* playerController = Cast<APW_PlayerController>(It->Get());
		if (playerController)
		{
			SavePlayerInventoryData(playerController);
		}
	}

	for (auto& playerInventory : _gameInstance->GetGameSessionData()._playersInventoryData._playerInventorys)
	{
		for (auto& itemID : playerInventory._itemIDs)
		{
			DEBUG_STRING(playerInventory._playerName  + " : " + itemID);
		}
	}
}

void APW_GameMode::SavePlayerInventoryData(APW_PlayerController* playerController) const
{
	//DEBUG_STRING("Starting Save Player Inventory Data for " +  playerController->GetPlayerName() + " - " + FString::FromInt( playerController->GetInventoryItemIDs().Num()));
	_gameInstance->GetGameSessionData()._playersInventoryData.AddInventory(playerController->GetPlayerName(), playerController->GetInventoryItemIDs(), playerController->GetSelectedSlotIndex());
}

void APW_GameMode::TriggerPlayersAnnouncement(const FString& announcement, FColor color, float duration) const
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APW_PlayerController* playerController = Cast<APW_PlayerController>(It->Get()))
		{
			playerController->ClientShowAnnocement( announcement, color, duration);
		}
	}
}

void APW_GameMode::TriggerPlayersLoadingScreen(const FMapDataEntry& mapdata)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APW_PlayerController* playerController = Cast<APW_PlayerController>(It->Get()))
		{
			playerController->ClientShowLoadingMenu(mapdata);
		}
	}
}

void APW_GameMode::TriggerNotification(const FNotificationEntry& notification) const
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APW_PlayerController* playerController = Cast<APW_PlayerController>(It->Get()))
		{
			playerController->TriggerNotification(notification);
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

FGameSessionData& APW_GameMode::GetGameSessionData()
{
	FGameSessionData sessionData;
	_gameInstance = Cast<UPW_GameInstance>(GetGameInstance());
	if(_gameInstance)
	{
		return _gameInstance->GetGameSessionData();
	}
	DEBUG_STRING("_gameInstance not Found");
	return sessionData;
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
		//NotifyPlayersOfMoney();
		//DEBUG_STRING("LoadGameSessionData Found");
		//LoadAllPlayersInventoryData();
	}
}

void APW_GameMode::PlayerEliminated(APW_Character* ElimmedCharacter, APW_PlayerController* VictimController,AController* AttackerController)
{

}

int32 APW_GameMode::GetRandomColorIndex()
{
	int value = 0;
	const int randomIndex = FMath::RandRange(0, _colorIndexes.Num() - 1);
	value = _colorIndexes[randomIndex];
	_colorIndexes.RemoveAt(randomIndex);
	return value;
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
