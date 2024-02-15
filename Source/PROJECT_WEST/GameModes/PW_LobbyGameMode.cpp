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

APW_LobbyGameMode::APW_LobbyGameMode()
{
	bUseSeamlessTravel = true;
	_mapPath = "";
	_bountySystemComponent = CreateDefaultSubobject<UBountySystemComponent>( "BountySystemComponent" );
}

void APW_LobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	_bountyBoard = Cast<APW_BountyBoard>(UGameplayStatics::GetActorOfClass(GetWorld(), APW_BountyBoard::StaticClass()));
	if ( _bountyBoard)
	{
		APW_GameState* gameState = GetGameState<APW_GameState>();
		if (gameState)
		{
			APW_PlayerController* localPlayerController = gameState->GetLocalPlayerController();
			if (localPlayerController)
			{

			}
		}
	}

	_transitionPortal = Cast<APW_TransitionPortal>(UGameplayStatics::GetActorOfClass(GetWorld(), APW_TransitionPortal::StaticClass()));
	if (_transitionPortal)
	{
		_transitionPortal->OnSuccess.AddDynamic(this, &APW_LobbyGameMode::OnTransitionCompleted);
	}
	
	ToggleSessionLock(false);

	_spawnPointsManager = Cast<APW_SpawnPointsManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APW_SpawnPointsManager::StaticClass()));
	_spawnPointsHandlerComponent = _spawnPointsManager ? _spawnPointsManager->GetSpawnPointsHandlerComponent() : nullptr;
	
	int bountyCollectorInterval = 1;
	if (_gameInstance->GetGameSessionData()._dayIndex % bountyCollectorInterval == 0)
	{
		TriggerDebtCollector();
	}
}

void APW_LobbyGameMode::LoadGameSessionData()
{
	Super::LoadGameSessionData();
	_gameInstance->GetGameSessionData()._dayIndex++;
}

void APW_LobbyGameMode::OnTransitionCompleted()
{
	ToggleAllPlayersInput(false);
	FBountyDataEntry bounty = _bountyBoard->GetBountyWithHighestVotes();
	_gameInstance->GetGameSessionData()._bountyDataEntry = bounty;
	RemoveMoney(bounty._bountyCost);
	ServerTravel(bounty._bountyMapDataEntry._bountyMapPath);
}

void APW_LobbyGameMode::TriggerDebtCollector()
{
	if (!_debtCollectorClass || !_spawnPointsHandlerComponent) return;
	
	_debtCollector = GetWorld()->SpawnActor<APW_DebtCollector>(_debtCollectorClass, _spawnPointsHandlerComponent->_debtCollectorSpawnPoint.GetRandomSpawnPoint(), FRotator::ZeroRotator);
	if (_debtCollector)
	{
		_debtCollector->SetDebtAmount(_gameInstance->GetGameSessionData()._dayIndex);
	}
}