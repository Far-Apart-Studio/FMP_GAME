// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_LobbyGameMode.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "PROJECT_WEST/Bounty System/BountySystemComponent.h"
#include "PROJECT_WEST/Bounty System/PW_BountyBoard.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/Gameplay/PW_GameInstance.h"
#include "PROJECT_WEST/PlayerState/PW_PlayerState.h"
#include "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/GameState/PW_GameState.h"
#include "PROJECT_WEST/PlayerController/PW_PlayerController.h"
#include "PROJECT_WEST/Bounty System/PW_TransitionPortal.h"
#include "PROJECT_WEST/Gameplay/PW_DebtCollector.h"
#include "PROJECT_WEST/Gameplay/Spawner/PW_SpawnPointsManager.h"
#include "PROJECT_WEST/Items/PW_Currency.h"

APW_LobbyGameMode::APW_LobbyGameMode()
{
	bUseSeamlessTravel = true;
	_bountySystemComponent = CreateDefaultSubobject<UBountySystemComponent>( "BountySystemComponent" );
	_bountyCollectorDayInterval = 4;
}

void APW_LobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	

}

void APW_LobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

void APW_LobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	_bountyBoard = Cast<APW_BountyBoard>(UGameplayStatics::GetActorOfClass(GetWorld(), APW_BountyBoard::StaticClass()));
	if (! _bountyBoard)
	{
		DEBUG_STRING( "APW_LobbyGameMode::BeginPlay _bountyBoard not found" );
	}

	_transitionPortal = Cast<APW_TransitionPortal>(UGameplayStatics::GetActorOfClass(GetWorld(), APW_TransitionPortal::StaticClass()));
	if (_transitionPortal)
	{
		_transitionPortal->OnSuccess.AddDynamic(this, &APW_LobbyGameMode::OnTransitionCompleted);
	}
	
	ToggleSessionLock(false);

	_spawnPointsManager = Cast<APW_SpawnPointsManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APW_SpawnPointsManager::StaticClass()));

	if (!_spawnPointsManager)
	{
		DEBUG_STRING ("APW_LobbyGameMode::BeginPlay _spawnPointsManager not found");
	}
	
	if (_gameInstance->GetGameSessionData()._dayIndex % _bountyCollectorDayInterval == 0)
	{
		_bountyBoard->RefereshBountyDataList();
		TriggerDebtCollector();
	}
	else
	{
		_bountyBoard->ToggleActivation(true);
	}
}

void APW_LobbyGameMode::LoadGameSessionData()
{
	Super::LoadGameSessionData();
	SetDay(_gameInstance->GetGameSessionData()._dayIndex + 1);
}

void APW_LobbyGameMode::OnTransitionCompleted()
{
	ToggleAllPlayersInput(false);
	FBountyDataEntry bounty = _bountyBoard->GetBountyWithHighestVotes();
	_gameInstance->GetGameSessionData()._bountyDataEntry = bounty;
	SaveAllPlayersInventoryData();
	RemoveMoney(bounty._bountyCost);
	ServerTravelMapEnty(bounty._bountyMapDataEntry);
}

void APW_LobbyGameMode::TriggerDebtCollector()
{
	if (!_debtCollectorClass )
	{
		DEBUG_STRING ("APW_LobbyGameMode::TriggerDebtCollector _debtCollectorClass is not valid");
		return;
	}

	if (!_spawnPointsManager)
	{
		DEBUG_STRING ("APW_LobbyGameMode::TriggerDebtCollector _spawnPointsHandlerComponent is not valid");
		return;
	}
	
	_debtCollector = GetWorld()->SpawnActor<APW_DebtCollector>(_debtCollectorClass, _spawnPointsManager->GetDebtCollectorSpawnPoint(), FRotator::ZeroRotator);
	if (_debtCollector)
	{
		DEBUG_STRING ("APW_LobbyGameMode::TriggerDebtCollector _debtCollector spawned");
		_debtCollector->SetDebtAmount(_gameInstance->GetGameSessionData()._dayIndex);
		
		FNotificationEntry notification;
		notification._notificationType = ENotificationType::EInfo;
		notification._notificationText = "Debt collector has arrived";
		TriggerNotification(notification);
	}
}

void APW_LobbyGameMode::TryPayDebtCollector()
{
	if (_gameInstance->GetGameSessionData()._money >= _debtCollector->GetDebtAmount())
	{
		RemoveMoney(_debtCollector->GetDebtAmount());
		_bountyBoard->ToggleActivation(true);
		_debtCollector->OnInteracted(true);
		_debtCollector->Destroy();

		
		FNotificationEntry notification;
		notification._notificationType = ENotificationType::EInfo;
		notification._notificationText = "Debt paid";
		TriggerNotification(notification);
	}
	else
	{
		TriggerPlayersAnnouncement("Not enough money to pay the debt collector", FColor::Red, 5.0f);
		ResetSessionData();
		_debtCollector->OnInteracted(false);
		_debtCollector->Destroy();
	}
}

void APW_LobbyGameMode::ResetSessionData()
{
	if (_gameInstance)
	{
		_gameInstance->GetGameSessionData()._money = 0;
		_gameInstance->GetGameSessionData()._dayIndex = 0;
		_gameInstance->GetGameSessionData()._bountyDataList.Empty();
		NotifyPlayersOfMoney();
		NofigyPlayersOfDay();
		RestartGame();
	}
}