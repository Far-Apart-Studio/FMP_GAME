// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_BountyGameMode.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "PROJECT_WEST/GameState/PW_GameState.h"
#include "PROJECT_WEST/PlayerState/PW_PlayerState.h"
#include "PROJECT_WEST/PlayerController/PW_PlayerController.h"
#include "PROJECT_WEST/Items/PW_Lantern.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/Gameplay/Spawner/PW_SpawnPointsManager.h"
#include "Kismet/GameplayStatics.h"
#include "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/PW_HealthComponent.h"
#include "PROJECT_WEST/Items/PW_BountyHead.h"
#include "PROJECT_WEST/Bounty System/PW_ExtractionPoint.h"
#include "PROJECT_WEST/Gameplay/PW_GameInstance.h"
#include "PROJECT_WEST/Gameplay/Spawner/PW_AutoEnemySpawner.h"
#include "PROJECT_WEST/Items/PW_Currency.h"
#include "PROJECT_WEST/SideObjective/PW_SideObjectiveManager.h"

namespace MatchState
{
	const FName Lose = FName("Lose");
	const FName Win = FName("Win");
	const FName Cooldown = FName("Cooldown");
}

APW_BountyGameMode::APW_BountyGameMode()
{
	bUseSeamlessTravel = true;
	_mapPath = "";
	_matchStartTime = 0.f;
	_matchTime = 120.f;
	_mathEndCooldownTime = 10.f;
	_numOfCurrencies = 1;
	//bDelayedStart = true; use if you want to delay the start of the game
}

void APW_BountyGameMode::BeginPlay()
{
	Super::BeginPlay();

	ToggleSessionLock(true);
	
	_levelStartTime = GetWorld()->GetTimeSeconds();
		
	_spawnPointsManager = Cast<APW_SpawnPointsManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APW_SpawnPointsManager::StaticClass()));

	_sideObjectiveManager = Cast<APW_SideObjectiveManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APW_SideObjectiveManager::StaticClass()));
	_sideObjectiveManager->_onObjectiveCompleted.AddDynamic(this, &APW_BountyGameMode::OnSideObjectiveCompleted);
	
	StartMatch();
	
	SpawnBountyEnemy();
	SpawnLantern();
	SpawnExtractionPoint();
	SpawnCurrencies();
	
	_matchStartTime = GetWorld()->GetTimeSeconds();

	//DEBUG_STRING( "APW_BountyGameMode::BeginPlay" );

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APW_PlayerController* playerController = Cast<APW_PlayerController>(It->Get()))
		{
			playerController->ClientJoinMidGame(MatchState, _matchTime, _levelStartTime, _mathEndCooldownTime);
		}
	}

	OnInitializeComplete();
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

void APW_BountyGameMode::HandleStateTimer()
{
	if (MatchState == MatchState::InProgress)
	{
		_countdownTime =  _matchTime - GetWorld()->GetTimeSeconds() + _matchStartTime;
		
		if (_countdownTime <= 0.f)
		{
			OnTimeUp();
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
	if(MatchState == MatchState::Cooldown || MatchState == MatchState::LeavingMap) return;
	_mathEndCooldownTime += GetWorld()->GetTimeSeconds() - _levelStartTime;
	_bountySuccessful = false;
	ToggleAllPlayersInput(false);
	SaveAllPlayersInventoryData();
	SetMatchState(MatchState::Cooldown);
}

void APW_BountyGameMode::OnTimeUp()
{
	DEBUG_STRING( "Time is up" );
	BountyFailed();
}

void APW_BountyGameMode::BountySuccessful()
{
	_bountySuccessful = true;
	ToggleAllPlayersInput(false);
	SaveAllPlayersInventoryData();
	SetMatchState(MatchState::Cooldown);

	// Give money to players
	AddMoney(_gameInstance->GetGameSessionData()._bountyDataEntry._bountyReward);
	DEBUG_STRING("Bounty successful. Money Gained: " + FString::FromInt(_gameInstance->GetGameSessionData()._bountyDataEntry._bountyReward));
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
		FNotificationEntry notification;
		notification._notificationType = ENotificationType::EDeath;
		notification._playerNameText = victimState ? victimState->GetPlayerName() : "";
		notification._notificationText = "has been eliminated";
		TriggerNotification(notification);
		
		APW_PlayerController* host = GetAnyPlayerAlive();
		if(host)
		{
			//DEBUG_STRING( "Player alive"  + host->GetName() );
			if (VictimController)
			{
				VictimController->ClientTogglePlayerInput(false);
				VictimController->SpectatePlayer(host);
				VictimController->SpectateModeActivated();
			}
		}
		else
		{
			_gameInstance->GetGameSessionData()._escapedPlayers.Empty();
			BountyFailed();
		}
	}
}

void APW_BountyGameMode::LoadGameSessionData()
{
	Super::LoadGameSessionData();
	
	if (_gameInstance)
	{
		//DEBUG_STRING("Bounty cost: " + FString::FromInt(_gameInstance->GetGameSessionData()._bountyDataEntry._bountyCost));
	}
}

void APW_BountyGameMode::SpawnLantern()
{
	if(!_spawnPointsManager || !_lanternClass) return;
	_lantern = GetWorld()->SpawnActor<APW_Lantern>(_lanternClass);
	if (_lantern)
	{
		_lantern->SetActorLocation(_spawnPointsManager->GetLanternSpawnPoint());
		_lantern->SetActorRotation(FRotator(0, 0, 0));
		_lantern->SetOwner(nullptr);

		if (_bountyEnemy)
		{
			_lantern->SetTarget(_bountyEnemy);
			//DEBUG_STRING ( "Lantern spawned and set target" );
		}
	}
}

void APW_BountyGameMode::SpawnBountyPortal()
{
	if (const AActor* bountyPortalExit = UGameplayStatics::GetActorOfClass(GetWorld(), _bountyPortalClass))
	{
		_bountyPortalEntrance = GetWorld()->SpawnActor<AActor>(_bountyPortalClass);
		_bountyPortalEntrance->SetActorLocation(bountyPortalExit->GetActorLocation());
		_bountyPortalEntrance->SetActorRotation(bountyPortalExit->GetActorRotation());
		_bountyPortalEntrance->SetOwner(nullptr);
	}
}

void APW_BountyGameMode::OnBountyDead(AActor* OwnerActor, AActor* DamageCauser, AController* DamageCauserController)
{
	if (_bountySuccessful) return;
	SpawnBountyHead();

	FNotificationEntry notification;
	notification._notificationType = ENotificationType::EElimination;
	if (const APW_Character* bountyKiller = Cast<APW_Character>(DamageCauser))
	{
		notification._playerNameText = bountyKiller->GetPlayerName();
		notification._notificationText = "has eliminated the bounty";
	}
	else
	{
		notification._playerNameText = "";
		notification._notificationText = "The bounty has been eliminated";
	}
	
	TriggerNotification(notification);
}

void APW_BountyGameMode::SpawnBountyEnemy()
{
	if (!_spawnPointsManager || !_bountyEnemyClass) return;
	_bountyEnemy = GetWorld()->SpawnActor<AActor>(_bountyEnemyClass,_spawnPointsManager->GetBountySpawnPoint(), FRotator(0, 0, 0));
	if (_bountyEnemy)
	{
		if (UPW_HealthComponent* healthComponent = _bountyEnemy->FindComponentByClass<UPW_HealthComponent>())
		{
			//DEBUG_STRING( "Bounty enemy spawned and health component found" );
			healthComponent->OnDeathGlobal.AddDynamic(this, &APW_BountyGameMode::OnBountyDead);
		}
	}
}

void APW_BountyGameMode::SpawnBountyHead()
{
	if (!_bountyHeadClass) return;
	_bountyHead = GetWorld()->SpawnActor<AActor>(_bountyHeadClass);
	if (_bountyHead)
	{
		_bountyHead->SetActorLocation(_bountyEnemy->GetActorLocation());
		_bountyHead->SetActorRotation(FRotator(0, 0, 0));
		_bountyHead->SetOwner(nullptr);
	}
}

void APW_BountyGameMode::OnActivateExtrationPoint(bool bWinCondition, TArray<FString> escapedPlayers)
{
	_gameInstance->GetGameSessionData()._escapedPlayers = escapedPlayers;
	
	if (bWinCondition)
	{
		BountySuccessful();
	}
	else
	{
		BountyFailed();
	}
}

void APW_BountyGameMode::OnPlayerTriggerExtractionPoint(APW_Character* Player)
{
	FNotificationEntry notification;
	notification._notificationType = ENotificationType::EInfo;
	if (Player)
	{
		notification._playerNameText = Player->GetPlayerName();
		notification._notificationText = "has triggered the extraction point";
	}
	else
	{
		notification._playerNameText = "";
		notification._notificationText = "The extraction point has been triggered";
	}
	TriggerNotification(notification);
}

void APW_BountyGameMode::SpawnExtractionPoint()
{
	if (!_spawnPointsManager || !_extractionPointClass) return;
	_extractionPoint = GetWorld()->SpawnActor<APW_ExtractionPoint>(_extractionPointClass);
	if (_extractionPoint)
	{
		_extractionPoint->SetActorLocation(_spawnPointsManager->GetExtractionSpawnPoint());
		_extractionPoint->SetActorRotation(FRotator(0, 0, 0));
		_extractionPoint->SetOwner(nullptr);
		_extractionPoint->OnWinConditionMet.AddDynamic(this, &APW_BountyGameMode::OnActivateExtrationPoint);
		_extractionPoint->OnPlayerTrigger.AddDynamic(this, &APW_BountyGameMode::OnPlayerTriggerExtractionPoint);
	}
}

void APW_BountyGameMode::SpawnAutoEnemySpawner(ACharacter* character)
{
	if (!_autoSpawnerClass) return;
	APW_AutoEnemySpawner* enemySpawner = GetWorld()->SpawnActor<APW_AutoEnemySpawner>(_autoSpawnerClass);
	if (enemySpawner)
	{
		enemySpawner->SetCharacter(character);
	}
}

void APW_BountyGameMode::EnemyEliminated(AActor* OwnerActor,AActor* DamageCauser, AController* DamageCauserController)
{
	_enemyCount--;
}

void APW_BountyGameMode::OnSideObjectiveCompleted(APW_SideObjective* ComplectedObjective)
{
	AddMoney(ComplectedObjective->GetObjectiveData()._sideObjectiveInfo._objectiveReward);
}

void APW_BountyGameMode::SpawnCurrencies()
{
	if (!_currencyClass || !_spawnPointsManager) return;
	TArray<FVector> currencySpawnPoints = _spawnPointsManager->GetRandomCurrencySpawnPoints(_numOfCurrencies);
	for (FVector spawnPoint : currencySpawnPoints)
	{
		APW_Currency* currency = GetWorld()->SpawnActor<APW_Currency>(_currencyClass);
		if (currency)
		{
			currency->SetActorLocation(spawnPoint);
			currency->SetActorRotation(FRotator(0, 0, 0));
			currency->SetOwner(nullptr);
		}
	}
}
