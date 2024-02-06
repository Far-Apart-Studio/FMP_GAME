// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_BountyGameMode.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "PROJECT_WEST/GameState/PW_GameState.h"
#include "PROJECT_WEST/PlayerState/PW_PlayerState.h"
#include "PROJECT_WEST/PlayerController/PW_PlayerController.h"
#include "PROJECT_WEST/Items/PW_Lantern.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/Gameplay/PW_SpawnPointsManager.h"
#include "PROJECT_WEST/Gameplay/PW_SpawnPointsHandlerComponent.h"
#include "Kismet/GameplayStatics.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

APW_BountyGameMode::APW_BountyGameMode()
{
	bUseSeamlessTravel = true;
	_mapPath = "";
	_matchStartTime = 0.f;
	_matchTime = 120.f;
	_mathEndCooldownTime = 10.f;

	//bDelayedStart = true; use if you want to delay the start of the game
}

void APW_BountyGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	_levelStartTime = GetWorld()->GetTimeSeconds();
	
	StartMatch();
	
	_spawnPointsManager = Cast<APW_SpawnPointsManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APW_SpawnPointsManager::StaticClass()));
	_spawnPointsHandlerComponent = _spawnPointsManager ? _spawnPointsManager->GetSpawnPointsHandlerComponent() : nullptr;

	
	SpawnBountyEnemy();
	SpawnLantern();
	
	_matchStartTime = GetWorld()->GetTimeSeconds();

	DEBUG_STRING( "APW_BountyGameMode::BeginPlay" );

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APW_PlayerController* playerController = Cast<APW_PlayerController>(It->Get());
		if (playerController)
		{
			playerController->ClientJoinMidGame(MatchState, _matchTime, _levelStartTime, _mathEndCooldownTime);
		}
	}
}

void APW_BountyGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	HandleStateTimer();
	
	//FString matchStateString = FName::NameToDisplayString( MatchState.ToString(), false );
	//DEBUG_STRING( "MatchState: " + matchStateString );
}
void APW_BountyGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APW_PlayerController* playerController = Cast<APW_PlayerController>(It->Get());
		if (playerController)
		{
			playerController->OnMatchStateSet( MatchState );
		}
	}

	if (MatchState == MatchState::LeavingMap)
	{
		ServerTravel(_mapPath);
	}
}

void APW_BountyGameMode::ToggleAllPlayersInput(bool bEnable)
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

void APW_BountyGameMode::HandleStateTimer()
{
	if (MatchState == MatchState::InProgress)
	{
		_countdownTime =  _matchTime - GetWorld()->GetTimeSeconds() + _matchStartTime;
		
		if (_countdownTime <= 0.f)
		{
			DEBUG_STRING( "Time is up" );
			BountyFailed();
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		_countdownTime = _mathEndCooldownTime  - GetWorld()->GetTimeSeconds() + _matchStartTime;
		if (_countdownTime <= 0.f)
		{
			DEBUG_STRING( "Cooldown is up" );
			SetMatchState(MatchState::LeavingMap);
			//RestartGame();
		}
	}
}

void APW_BountyGameMode::BountyFailed()
{
	_mathEndCooldownTime += GetWorld()->GetTimeSeconds();
	_bountySuccessful = false;
	ToggleAllPlayersInput(false);
	SetMatchState(MatchState::Cooldown);
	DEBUG_STRING( "BountyFailed"  + MatchState.ToString() );
}

APW_PlayerController* APW_BountyGameMode::GetAnyPlayerAlive()
{
	APW_PlayerController* playerAlive = nullptr;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APW_PlayerController* playerController = Cast<APW_PlayerController>(It->Get());
		if (playerController && playerController->IsAlive())
		{
			playerAlive = playerController; 
		}
	}
	return playerAlive;
}

void APW_BountyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	APW_PlayerController* playerController = Cast<APW_PlayerController>(NewPlayer);
	if (playerController)
	{
		//DEBUG_STRING( "APW_BountyGameMode::PostLogin: playerController is valid" );
		playerController->ClientJoinMidGame(MatchState, _matchTime, _levelStartTime, _mathEndCooldownTime);
	}
	else
	{
		//DEBUG_STRING( "APW_BountyGameMode::PostLogin: playerController is nullptr" );
	}

	DEBUG_STRING( "APW_BountyGameMode::PostLogin: " + NewPlayer->GetName() );
}

void APW_BountyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

void APW_BountyGameMode::PlayerEliminated(APW_Character* ElimmedCharacter, APW_PlayerController* VictimController, AController* AttackerController)
{
	Super::PlayerEliminated(ElimmedCharacter, VictimController, AttackerController);

	APW_PlayerState* victimState = VictimController ? Cast<APW_PlayerState>(VictimController->PlayerState) : nullptr;

	if (ElimmedCharacter)
	{
		
		APW_PlayerController* host = GetAnyPlayerAlive();

		if(host)
		{
			DEBUG_STRING( "Player alive"  + host->GetName() );
			if (VictimController)
			{
				VictimController->ClientTogglePlayerInput(false);
				VictimController->SpectatePlayer(host);
			}
		}
		else
		{
			DEBUG_STRING( "No player alive" );
			BountyFailed();
		}
	}
}

void APW_BountyGameMode::EnemyEliminated(APW_Character* AttackerCharacter, APW_PlayerController* AttackerController)
{
	APW_PlayerState* attackerState = AttackerController ? Cast<APW_PlayerState>(AttackerController->PlayerState) : nullptr;

	APW_GameState* gameState = GetGameState<APW_GameState>();
	if (gameState)
	{
		gameState->UpdateTopScore(AttackerController->GetPlayerState<APW_PlayerState>());
	}
}

void APW_BountyGameMode::BountySuccessful()
{
	_bountySuccessful = true;
	ToggleAllPlayersInput(false);
	SetMatchState(MatchState::Cooldown);
}

void APW_BountyGameMode::SpawnLantern()
{
	if(!_spawnPointsHandlerComponent || !_lanternClass) return;
	_lantern = GetWorld()->SpawnActor<APW_Lantern>(_lanternClass);
	if (_lantern)
	{
		_lantern->SetActorLocation(_spawnPointsHandlerComponent->GetLanternSpawnPoint());
		_lantern->SetActorRotation(FRotator(0, 0, 0));
		_lantern->SetOwner(nullptr);

		if (_bountyEnemy)
		{
			_lantern->SetTarget( _bountyEnemy);
			DEBUG_STRING ( "Lantern spawned and set target" );
		}
		else
		{
			DEBUG_STRING ( "Lantern spawned but no target" );
		}
	}
}

void APW_BountyGameMode::SpawnBountyEnemy()
{
	if (!_spawnPointsHandlerComponent || !_bountyEnemyClass) return;
	_bountyEnemy = GetWorld()->SpawnActor<AActor>(_bountyEnemyClass);
	if (_bountyEnemy)
	{
		_bountyEnemy->SetActorLocation(_spawnPointsHandlerComponent->GetBountySpawnPoint());
		_bountyEnemy->SetActorRotation(FRotator(0, 0, 0));
		_bountyEnemy->SetOwner(nullptr);
		DEBUG_STRING( "Bounty enemy spawned" );
	}
}
