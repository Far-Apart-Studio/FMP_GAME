// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_LobbyGameMode.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "PROJECT_WEST/Bounty System/BountySystemComponent.h"
#include "PROJECT_WEST/Bounty System/PW_BountyBoard.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/Gameplay/PW_GameInstance.h"
#include "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/GameState/PW_GameState.h"
#include "PROJECT_WEST/PlayerController/PW_PlayerController.h"
#include "PROJECT_WEST/Bounty System/PW_TransitionPortal.h"
#include "PROJECT_WEST/Gameplay/PW_DebtCollector.h"
#include "PROJECT_WEST/Gameplay/PW_SpawnPointsManager.h"
#include "PROJECT_WEST/Items/PW_Currency.h"

APW_LobbyGameMode::APW_LobbyGameMode()
{
	bUseSeamlessTravel = true;
	_bountySystemComponent = CreateDefaultSubobject<UBountySystemComponent>( "BountySystemComponent" );
	_bountyCollectorDayInterval = 4;
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
	_spawnPointsHandlerComponent = _spawnPointsManager ? _spawnPointsManager->GetSpawnPointsHandlerComponent() : nullptr;
	
	if (_gameInstance->GetGameSessionData()._dayIndex % _bountyCollectorDayInterval == 0)
	{
		_bountyBoard->RefereshBountyDataList();
		TriggerDebtCollector();
	}
	else
	{
		_bountyBoard->ToggleActivation(true);
	}

	// to be removed
	SpawnCurrency();
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
	RemoveMoney(bounty._bountyCost);
	ServerTravel(bounty._bountyMapDataEntry._bountyMapPath);
}

void APW_LobbyGameMode::OnDebtCollectorInteract(bool bSuccess)
{
	if (bSuccess)
	{
		_bountyBoard->ToggleActivation(true);
		_debtCollector->Destroy();
	}
	else
	{
		TriggerPlayersAnnouncement("Not enough money to pay the debt collector", FColor::Red, 5.0f);
		ResetSessionData();
	}
}

void APW_LobbyGameMode::TriggerDebtCollector()
{
	if (!_debtCollectorClass || !_spawnPointsHandlerComponent) return;
	
	_debtCollector = GetWorld()->SpawnActor<APW_DebtCollector>(_debtCollectorClass, _spawnPointsHandlerComponent->_debtCollectorSpawnPoint.GetRandomSpawnPoint(), FRotator::ZeroRotator);
	if (_debtCollector)
	{
		_debtCollector->SetDebtAmount(_gameInstance->GetGameSessionData()._dayIndex);
		_debtCollector->_onInteract.AddDynamic(this, &APW_LobbyGameMode::OnDebtCollectorInteract);
	}
}

void APW_LobbyGameMode::OnCurrencyCollected(APW_Currency* Currency)
{
	Currency->Destroy();
}

void APW_LobbyGameMode::SpawnCurrency()
{
	if (!_currencyClass || !_spawnPointsHandlerComponent) return;
	APW_Currency* currency = GetWorld()->SpawnActor<APW_Currency>(_currencyClass, _spawnPointsHandlerComponent->_currencySpawnPoint.GetRandomSpawnPoint(), FRotator::ZeroRotator);
	if (currency)
	{
		//currency->_onCollected.AddDynamic(this, &APW_LobbyGameMode::OnCurrencyCollected);
		DEBUG_STRING( "Currency SpawnCurrency" );
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
