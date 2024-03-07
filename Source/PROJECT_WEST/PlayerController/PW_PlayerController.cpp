// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_PlayerController.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "PROJECT_WEST/PW_Character.h"
#include "GameFramework/PlayerState.h"
#include "PROJECT_WEST/HUD/PW_HUD.h"
#include "PROJECT_WEST/HUD/PW_CharacterOverlayWidget.h"
#include "Net/UnrealNetwork.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/GameModes/PW_GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "PROJECT_WEST/GameModes/PW_BountyGameMode.h"
#include "PROJECT_WEST/GameModes/PW_LobbyGameMode.h"
#include "PROJECT_WEST/PW_ItemHandlerComponent.h"
#include "PROJECT_WEST/Bounty System/PW_BountyBoard.h"
#include "PROJECT_WEST/Gameplay/PW_GameInstance.h"
#include "PROJECT_WEST/PW_ConsoleCommandManager.h"
#include "PROJECT_WEST/HUD/PW_AnnouncementWidget.h"
#include "PROJECT_WEST/Items/PW_Currency.h"
#include "PROJECT_WEST/PW_InventoryHandler.h"

APW_PlayerController::APW_PlayerController()
{
	_matchTime = 0.f;
	_clientServerDelta = 0;
	_timeSyncFrequency = 5;
	_timeSyncRuningTime = 0;
	_consoleCommandManager = CreateDefaultSubobject<UPW_ConsoleCommandManager>(TEXT("ConsoleCommandManager"));
}

void APW_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// called when the player controller possesses a pawn
	
	ClientAddCharacterOverlayWidget();
	SetNewPlayerName();
	ClientOnLevelChanged();
	SpawnAutoEnemySpawner();
	//LoadInventoryData();

	//DEBUG_STRING( "APW_PlayerController OnPossess : VOTED INDEX" + FString::FromInt(_votedBountyIndex) + " HAS VOTED : " + FString::FromInt(_hasVoted) );	
}

void APW_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	//DEBUG_STRING ( "APW_PlayerController BeginPlay" );

	//ServerCheckMatchState();
	
	_highPingRunningTime = 0;
	_highPingDuration = 5;
	_checkPingFrequency = 20;
	_highPingThreshold = 50;

	//ServerSetPlayerName();
}

void APW_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!IsLocalController()) return;

	if(_matchState == MatchState::InProgress || _matchState == MatchState::Cooldown)
	{
		SetHUDTime();
		SyncTimeWithServer(DeltaTime);
	}

	//HandleCheckPing(DeltaTime);
}

void APW_PlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME( APW_PlayerController, _matchState );
	DOREPLIFETIME( APW_PlayerController, _playerName );
}

void APW_PlayerController::ClientToggleGravity_Implementation(bool bEnable)
{
	const APW_Character* character = Cast<APW_Character>(GetPawn());
	if (character)
	{
		character->GetCharacterMovement()->SetMovementMode(bEnable ? EMovementMode::MOVE_Walking : EMovementMode::MOVE_Falling);
		character->GetCharacterMovement()->GravityScale = bEnable ? 1 : 0;
		character->GetMesh()->SetEnableGravity(bEnable);
	}
}

void APW_PlayerController::ClientActivateTrapMode_Implementation(AActor* trap)
{
	APW_Character* character = Cast<APW_Character>(GetPawn());
	if (character)
	{
		character->GetCharacterMovement()->Velocity = FVector::ZeroVector;
		character->GetCharacterMovement()->SetMovementMode( EMovementMode::MOVE_None);
		character->GetCharacterMovement()->GravityScale =  0;
		character->GetMesh()->SetEnableGravity(false);
		character->AttachToComponent(trap->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		character->SetActorLocation(trap->GetActorLocation());
	}
}

void APW_PlayerController::ClientDeactivateTrapMode_Implementation()
{
	
}

float APW_PlayerController::GetServerTime()
{
	if (HasAuthority()) return  GetWorld()->GetTimeSeconds();
	else return GetWorld()->GetTimeSeconds() + _clientServerDelta;
}

void APW_PlayerController::SpectatePlayer(APW_PlayerController* playerController)
{
	DropAllItems();
	SetViewTargetWithBlend( playerController->GetPawn(), 0.5f, EViewTargetBlendFunction::VTBlend_Cubic );
}

void APW_PlayerController::ClientMoneyValueChanged_Implementation(int32 money)
{
	_money = money;
}

void APW_PlayerController::ClientShowAnnocement_Implementation(const FString& message, FColor color, float duration)
{
	DisplayAccouncement(message, color, duration);
}

void APW_PlayerController::ClientDayChanged_Implementation(int32 day)
{
	_dayIndex = day;
}

bool APW_PlayerController::IsAlive()
{
	const APW_Character* character = Cast<APW_Character>(GetPawn());
	if (character)
	{
		return character->IsAlive();
	}
	return false;
}

void APW_PlayerController::PayDebtCollector()
{
	if (HasAuthority())
	{
		LocalPayDebtCollector();
	}
	else
	{
		ServerPayDebtCollector();
	}
}

void APW_PlayerController::ServerPayDebtCollector_Implementation()
{
	if (HasAuthority())
	{
		LocalPayDebtCollector();
	}
}

void APW_PlayerController::LocalPayDebtCollector()
{
	APW_LobbyGameMode* lobbyGameMode = GetWorld()->GetAuthGameMode<APW_LobbyGameMode>();
	if (lobbyGameMode)
	{
		lobbyGameMode->TryPayDebtCollector();
	}
}

void APW_PlayerController::SpawnAutoEnemySpawner()
{
	APW_Character* controlledCharacter = Cast<APW_Character>(GetPawn());
	if (!controlledCharacter) return;
	if (HasAuthority())
	{
		LocalSpawnAutoEnemySpawner(controlledCharacter);
	}
	else
	{
		ServerSpawnAutoEnemySpawner(controlledCharacter);
	}
}

void APW_PlayerController::ServerSpawnAutoEnemySpawner_Implementation(APW_Character* controlledCharacter)
{
	if (HasAuthority())
	{
		LocalSpawnAutoEnemySpawner(controlledCharacter);
	}
}

void APW_PlayerController::LocalSpawnAutoEnemySpawner(APW_Character* controlledCharacter)
{
	APW_BountyGameMode* gameMode = Cast<APW_BountyGameMode>(UGameplayStatics::GetGameMode(this));
	if (gameMode)
	{
		gameMode->SpawnAutoEnemySpawner(controlledCharacter);
	}
}

void APW_PlayerController::LoadInventoryItemsByID(const TArray<FString>& itemIDs)
{
	if(GetPawn())
	{
		DEBUG_STRING ("Pawn Found");
	}
	else
	{
		DEBUG_STRING( "No Pawn Found" );
		return;
	}
	
	UPW_InventoryHandler* inventoryHandler = Cast<UPW_InventoryHandler>(GetPawn()->GetComponentByClass(UPW_InventoryHandler::StaticClass()));
	if (inventoryHandler)
	{
		inventoryHandler->LoadItemsByID(itemIDs);
		DEBUG_STRING( "ClientLoadInventoryItemsByID_Implementation : " + FString::FromInt(itemIDs.Num()));
	}
}

TArray<FString> APW_PlayerController::GetInventoryItemIDs()
{
	TArray<FString > itemIDs;
	APW_Character* character = Cast<APW_Character>(GetPawn());
	if (character)
	{
		UPW_InventoryHandler* inventoryHandler = Cast<UPW_InventoryHandler>(character->GetComponentByClass(UPW_InventoryHandler::StaticClass()));
		if (inventoryHandler)
		{
			itemIDs = inventoryHandler->GetInventoryItemIDs();
		}
	}
	return itemIDs;
}

void APW_PlayerController::ClientLoadInventoryItems_Implementation(const TArray<APW_ItemObject*>& items)
{
	if(!GetPawn()) return;
	UPW_InventoryHandler* inventoryHandler = Cast<UPW_InventoryHandler>(GetPawn()->GetComponentByClass(UPW_InventoryHandler::StaticClass()));
	if (inventoryHandler)
	{
		inventoryHandler->LoadItems(items);
	}
}

void APW_PlayerController::ClientJoinMidGame_Implementation(FName stateOfMatch, float matchTime, float levelStartTime,float endMatchCountdown)
{
	_matchState = stateOfMatch;
	_matchTime = matchTime;
	_levelStartTime = levelStartTime;
	_endMatchCountdown = endMatchCountdown;
	OnMatchStateChanged();
}

void APW_PlayerController::ServerCheckMatchState_Implementation()
{
	APW_BountyGameMode* gameMode = Cast<APW_BountyGameMode>(UGameplayStatics::GetGameMode(this));
	if (gameMode)
	{
		DEBUG_STRING( "ServerCheckMatchState" );
		
		_matchTime = gameMode->GetMatchTime();
		_matchState = gameMode->GetMatchState();
		_levelStartTime = gameMode->GetLevelStartTime();
		_endMatchCountdown = gameMode->GetEndMatchCooldownTime();
		
		ClientJoinMidGame(_matchState, _matchTime, _levelStartTime, _endMatchCountdown);
	}
	else
	{
		DEBUG_STRING( "ServerCheckMatchState: No GameMode" );
	}
}

void APW_PlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		ServerRequestTime(GetWorld()->GetTimeSeconds());
	}
}

bool APW_PlayerController::ProcessConsoleExec(const TCHAR* Command, FOutputDevice& OutputDevice, UObject* Executor)
{
	bool isHandled = Super::ProcessConsoleExec(Command, OutputDevice, Executor);

	if (!isHandled && _consoleCommandManager != nullptr)
		isHandled = _consoleCommandManager->ProcessConsoleExec(Command, OutputDevice, Executor);

	return isHandled;
}

void APW_PlayerController::Destroyed()
{
	// Drop all items when player controller is destroyed
	if (_hasVoted && !HasAuthority())
	{

	}

	DropAllItems();
	Super::Destroyed();
}

void APW_PlayerController::DisplayAccouncement(const FString& message, FColor color, float duration)
{
	_announcementWidget = CreateWidget<UPW_AnnouncementWidget>(this, _announcementWidgetClass);
	if (_announcementWidget)
	{
		_announcementWidget->SetAnnouncementText(message,color);
		_announcementWidget->AddToViewport();
		GetWorldTimerManager().SetTimer(_announcementTimer, this, &APW_PlayerController::HideAccouncement, duration, false);
	}
}

void APW_PlayerController::HideAccouncement()
{
	if (_announcementWidget != nullptr)
	{
		_announcementWidget->RemoveFromViewport();
	}
}

void APW_PlayerController::ToggleHUDVisibility(bool bShow)
{
	if(_hud)
	{
		_hud->SetVisibility(bShow);
	}

	if (_characterOverlayWidget)
	{
		_characterOverlayWidget->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void APW_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void APW_PlayerController::ClientAddCharacterOverlayWidget_Implementation()
{
	_hud = GetHUD <APW_HUD>();
	
	if (_characterOverlayWidgetClass)
	{
		_characterOverlayWidget = CreateWidget<UPW_CharacterOverlayWidget>(this, _characterOverlayWidgetClass);
		if (_characterOverlayWidget != nullptr)
		{
			_characterOverlayWidget->AddToViewport();
		}
	}
}

void APW_PlayerController::SetHUDHealth(float health, float maxHealth)
{

}

void APW_PlayerController::SetHUDScore(float score)
{

}

void APW_PlayerController::SetMatchCountdown(float time)
{
	if (_characterOverlayWidget)
	{
		if (time < 0)
		{
			time = 0;
			_timeText = "00:00";
		}
		_timeText = ConvertToTime(time);
		//DEBUG_STRING ( ConvertToTime(time) );
	}
	else
	{
		DEBUG_STRING( "HUD is not valid" );
	}
}

void APW_PlayerController::SetMatchEndCountdown(float time)
{
	if (_characterOverlayWidget)
	{
		if (time < 0)
		{
			time = 0;
			_timeText = "00:00";
		}
		_timeText = ConvertToTime(time);
		//DEBUG_STRING( ConvertToTime(time) );
	}
	else
	{
		DEBUG_STRING( "HUD is not valid" );
	}
}

void APW_PlayerController::OnMatchStateSet(FName matchState) // Ran Only on Server
{
	_matchState = matchState;
	OnMatchStateChanged();
}

void APW_PlayerController::OnRep_MatchState() // Ran Only on Clients
{
	OnMatchStateChanged();
}

void APW_PlayerController::OnRep_PlayerName()
{
	_onNameChanged.Broadcast(_playerName);
}

void APW_PlayerController::OnMatchStateChanged()
{
	if (_matchState == MatchState::InProgress)
	{
		HandleMatchStarted();
	}
	else if (_matchState == MatchState::Cooldown)
	{
		HandleMatchCooldown();
	}
	else if (_matchState == MatchState::LeavingMap)
	{
		HandleMatchEnded();
	}
}

void APW_PlayerController::HandleMatchStarted()
{
	// show match started screen
}

void APW_PlayerController::HandleMatchCooldown()
{
	// show cooldown screen
	
	DisplayAccouncement("Match Ended, Returning to Lobby", FColor::Red, _endMatchCountdown);
	_endMatchCountdown += GetServerTime() - _levelStartTime;
}

void APW_PlayerController::HandleMatchEnded()
{
	
}

void APW_PlayerController::DropAllItems()
{
	APW_Character* character = Cast<APW_Character>(GetPawn());
	if (character)
	{
		UPW_ItemHandlerComponent* itemHandlerComponent = Cast<UPW_ItemHandlerComponent>( character->GetComponentByClass(UPW_ItemHandlerComponent::StaticClass()));
		if (itemHandlerComponent)
		{
			itemHandlerComponent->DoDropAllItems();
		}
	}
}

FString APW_PlayerController::ConvertToTime(float time)
{
	int32 minutes = FMath::FloorToInt(time / 60);
	int32 seconds = time - (minutes * 60);
	FString timeString = FString::Printf(TEXT("%02d:%02d"), minutes, seconds);
	return timeString;
}

void APW_PlayerController::TogglePlayerInput(bool bEnable)
{
	APW_Character* character = Cast<APW_Character>(GetPawn());
	if (character)
	{
		if (bEnable)
		{
			character->EnableInput(this);
		}
		else
		{
			character->DisableInput(this);
		}
	}
}

void APW_PlayerController::ClientTogglePlayerInput_Implementation(bool bEnable)
{
	TogglePlayerInput(bEnable);
}

void APW_PlayerController::ClientOnLevelChanged_Implementation()
{
	_votedBountyIndex = -1;
	_hasVoted = false;
	LoadGameSessionData();
	//DEBUG_STRING( "APW_PlayerController OnPossess Rest Data : VOTED INDEX" + FString::FromInt(_votedBountyIndex) + " HAS VOTED : " + FString::FromInt(_hasVoted) );
}

void APW_PlayerController::StartHighPingWarning()
{
	if (_characterOverlayWidget)
	{
		_characterOverlayWidget->StartHighPingWarning();
	}
}

void APW_PlayerController::StopHighPingWarning()
{
	if (_characterOverlayWidget)
	{
		_characterOverlayWidget->StopHighPingWarning();
	}
}

void APW_PlayerController::HandleCheckPing(float DeltaTime)
{
	if(!_hud) return;
	
	_highPingRunningTime += DeltaTime;
	if (_highPingRunningTime >= _checkPingFrequency)
	{
		PlayerState = PlayerState == nullptr ? GetPlayerState<APlayerState>() : PlayerState;
		if (PlayerState)
		{
			if (PlayerState->GetPingInMilliseconds() * 4 >= _highPingThreshold) //  ping is compressed, so we need to multiply it by 4
			{
				StartHighPingWarning();
				_pingAnimationRunningTime = 0;
			}
		}
		_highPingRunningTime = 0;
	}
	
	if (_characterOverlayWidget->IsHighPingWarningPlaying())
	{
		_pingAnimationRunningTime += DeltaTime;
		if (_pingAnimationRunningTime >= _highPingDuration)
		{
			StopHighPingWarning();
		}
	}
}

void APW_PlayerController::SetHUDTime()
{
	float timeleft = 0;
	
	if (_matchState == MatchState::InProgress)
	{
		timeleft = _matchTime - GetServerTime() + _levelStartTime;
	}
	else if (_matchState == MatchState::Cooldown)
	{
		timeleft = _endMatchCountdown - GetServerTime() + _levelStartTime;
	}

	uint32 secondsLeft = FMath::CeilToInt(timeleft);
	
	if (HasAuthority())
	{
		_bountyGameMode = _bountyGameMode ? Cast<APW_BountyGameMode>(UGameplayStatics::GetGameMode(this)) : nullptr;
		if (_bountyGameMode)
		{
			_levelStartTime = _bountyGameMode->GetLevelStartTime();
			secondsLeft = FMath::CeilToInt(_bountyGameMode->GetCountdownTime() + _levelStartTime);
		}
	}
	
	if (_countDownInt != secondsLeft)
	{
		_countDownInt = secondsLeft;
		if (_matchState == MatchState::InProgress)
		{
			SetMatchCountdown(secondsLeft);
		}
		else if (_matchState == MatchState::Cooldown)
		{
			SetMatchEndCountdown( secondsLeft );
		}
	}
}

void APW_PlayerController::SyncTimeWithServer(float deltaTime)
{
	_timeSyncRuningTime += deltaTime;
	if (IsLocalController() && _timeSyncRuningTime >= _timeSyncFrequency)
	{
		ServerRequestTime(GetWorld()->GetTimeSeconds());
		_timeSyncRuningTime = 0;
	}
}

void APW_PlayerController::SetNewPlayerName()
{
	if (HasAuthority())
	{
		APW_GameMode* gameMode = Cast<APW_GameMode>(UGameplayStatics::GetGameMode(this));
		if (gameMode)
		{
			_playerName = gameMode->GetPlayerName(this);
			MulticastSetPlayerName(gameMode->GetPlayerName(this));
		}
	}
	else
	{
		ServerSetPlayerName();
	}
}

void APW_PlayerController::ServerSetPlayerName_Implementation()
{
	APW_GameMode* gameMode = Cast<APW_GameMode>(UGameplayStatics::GetGameMode(this));
	if (gameMode)
	{
		MulticastSetPlayerName(gameMode->GetPlayerName(this));
	}
}

void APW_PlayerController::MulticastSetPlayerName_Implementation(const FString& playerName)
{
	APW_Character * character = Cast<APW_Character>(GetPawn());
	if (character)
	{
		_playerName = playerName;
		character->SetPlayerName(playerName);
	}
	else
	{
		DEBUG_STRING( "No Character Found");
	}
}

void APW_PlayerController::ClientSetPlayerName_Implementation(const FString& playerName)
{
	APW_Character * character = Cast<APW_Character>(GetPawn());
	if (character)
	{
		character->SetPlayerName(playerName);
	}
}

void APW_PlayerController::DoVoteToBounty(int32 bountyIndex)
{
	if(_voteSeverDelay) return;
	if (_hasVoted)
	{
		RemoveVoteFromBounty(_votedBountyIndex);
		return;
	}

	AddVoteToBounty(bountyIndex);
}

void APW_PlayerController::AddVoteToBounty(int32 bountyIndex)
{
	if (HasAuthority())
	{
		if(LocalAddVoteToBounty(bountyIndex))
		{
			_hasVoted = true;
			_votedBountyIndex = bountyIndex;
			VoteChangedDelegate.Broadcast(true, bountyIndex);
		}
	}
	else
	{
		_voteSeverDelay = true;
		ServerAddVoteToBounty(bountyIndex);
	}
}

void APW_PlayerController::RemoveVoteFromBounty(int32 bountyIndex)
{
	if (HasAuthority())
	{
		if(LocalRemoveVoteFromBounty(bountyIndex))
		{
			_hasVoted = false;
			VoteChangedDelegate.Broadcast(false, bountyIndex);
		}
	}
	else
	{
		_voteSeverDelay = true;
		ServerRemoveVoteFromBounty(bountyIndex);
	}
}

bool APW_PlayerController::LocalAddVoteToBounty(int32 bountyIndex)
{
	_lobbyGameMode = Cast<APW_LobbyGameMode>(UGameplayStatics::GetGameMode(this));

	if(!_lobbyGameMode)
	{
		DEBUG_STRING( "LocalAddVoteToBounty: No LobbyGameMode Found" );
		return false;
	}

	if (! _lobbyGameMode->GetBountyBoard())
	{
		DEBUG_STRING ( "LocalAddVoteToBounty: No BountyBoard Found" );
		return false;
	}

	if (_lobbyGameMode->GetBountyBoard()->GetBountyDataList().Num() == 0)
	{
		DEBUG_STRING( "LocalAddVoteToBounty: BountyDataList is empty" );
		return false;
	}
	
	FBountyDataEntry bounty = _lobbyGameMode->GetBountyBoard()->GetBountyDataList()[bountyIndex];
	if (bounty._bountyCost <= _lobbyGameMode->GetCurrentGameInstance()->GetGameSessionData()._money)
	{
		if(_lobbyGameMode->GetBountyBoard()->IsActivated())
		{
			_lobbyGameMode->GetBountyBoard()->AddVoteToBounty(bountyIndex);
			return true;
		}
	}
	
	return false;
}

bool APW_PlayerController::LocalRemoveVoteFromBounty(int32 bountyIndex)
{
	_lobbyGameMode = Cast<APW_LobbyGameMode>(UGameplayStatics::GetGameMode(this));

	if(!_lobbyGameMode)
	{
		DEBUG_STRING( "LocalAddVoteToBounty: No LobbyGameMode Found" );
		return false;
	}

	if (! _lobbyGameMode->GetBountyBoard())
	{
		DEBUG_STRING ( "LocalAddVoteToBounty: No BountyBoard Found" );
		return false;
	}

	if (_lobbyGameMode->GetBountyBoard()->GetBountyDataList().Num() == 0)
	{
		DEBUG_STRING( "LocalAddVoteToBounty: BountyDataList is empty" );
		return false;
	}
	
	if(_lobbyGameMode->GetBountyBoard()->IsActivated())
	{
		_lobbyGameMode->GetBountyBoard()->RemoveVoteFromBounty(bountyIndex);
		return true;
	}
	
	return false;
}

void APW_PlayerController::ServerAddVoteToBounty_Implementation(int32 bountyIndex)
{
	if (HasAuthority())
	{
		if(LocalAddVoteToBounty(bountyIndex))
		{
			ClientVoteToBounty(true, bountyIndex);
		}
	};
}

void APW_PlayerController::ServerRemoveVoteFromBounty_Implementation(int32 bountyIndex)
{
	if (HasAuthority())
	{
		if(LocalRemoveVoteFromBounty(bountyIndex))
		{
			ClientVoteToBounty(false, bountyIndex);
		}
	}
}

void APW_PlayerController::ClientVoteToBounty_Implementation(bool bSuccess, int32 bountyIndex)
{
	_hasVoted = bSuccess;
	_votedBountyIndex = bountyIndex;
	VoteChangedDelegate.Broadcast(bSuccess, bountyIndex);
	_voteSeverDelay = false;
}

void APW_PlayerController::ServerClearVote_Implementation()
{
	//DEBUG_STRING("ServerClearVote");
	
	if (HasAuthority())
	{
		_lobbyGameMode == nullptr ? _lobbyGameMode = Cast<APW_LobbyGameMode>(UGameplayStatics::GetGameMode(this)) : nullptr;
		if (_lobbyGameMode)
		{
			//DEBUG_STRING("ServerClearVote Done : " + FString::FromInt(_votedBountyIndex));
			_lobbyGameMode->GetBountyBoard()->RemoveVoteFromBounty(_votedBountyIndex);
		}
		else
		{
			DEBUG_STRING("ServerClearVote: No LobbyGameMode");
		}
	};
}

void APW_PlayerController::AddMoney(int32 amount)
{
	if (HasAuthority())
	{
		LocalAddMoney(amount);
	}
	else
	{
		SeverAddMoney(amount);
	}
}

void APW_PlayerController::RemoveMoney(int32 amount)
{
	if (HasAuthority())
	{
		LocalRemoveMoney(amount);
	}
	else
	{
		SeverRemoveMoney(amount);
	}
}

void APW_PlayerController::CollectCurrency(APW_Currency* currency)
{
	if (HasAuthority())
	{
		LocalCollectCurrency(currency);
	}
	else
	{
		ServerCollectCurrency(currency);
	}
}

void APW_PlayerController::ServerCollectCurrency_Implementation(APW_Currency* currency)
{
	if (HasAuthority())
	{
		LocalCollectCurrency(currency);
	}
}

void APW_PlayerController::LocalCollectCurrency(APW_Currency* currency)
{
	APW_GameMode* gameMode = Cast<APW_GameMode>(UGameplayStatics::GetGameMode(this));
	if (gameMode)
	{
		gameMode->CollectCurrency(currency);
	}
}

void APW_PlayerController::SeverAddMoney_Implementation(int32 amount)
{
	if (HasAuthority())
	{
		LocalAddMoney(amount);
	}
}

void APW_PlayerController::SeverRemoveMoney_Implementation(int32 amount)
{
	if (HasAuthority())
	{
		LocalRemoveMoney(amount);
	}
}

void APW_PlayerController::LocalAddMoney(int32 amount)
{
	APW_GameMode* gameMode = Cast<APW_GameMode>(UGameplayStatics::GetGameMode(this));
	if (gameMode)
	{
		gameMode->AddMoney(amount);
	}
}

void APW_PlayerController::LocalRemoveMoney(int32 amount)
{
	APW_GameMode* gameMode = Cast<APW_GameMode>(UGameplayStatics::GetGameMode(this));
	if (gameMode)
	{
		gameMode->RemoveMoney(amount);
	}
}

void APW_PlayerController::LoadGameSessionData()
{
	if(HasAuthority())
	{
		APW_GameMode* gameMode = Cast<APW_GameMode>(UGameplayStatics::GetGameMode(this));
		if (gameMode)
		{
			ClientLoadGameSessionData( gameMode->GetGameSessionData());
			
			//gameMode->LoadPlayerInventoryData(this);
		}
	}
	else
	{
		SeverLoadGameSessionData();
	}
}

void APW_PlayerController::SeverLoadGameSessionData_Implementation()
{
	if(HasAuthority())
	{
		APW_GameMode* gameMode = Cast<APW_GameMode>(UGameplayStatics::GetGameMode(this));
		if (gameMode)
		{
			ClientLoadGameSessionData( gameMode->GetGameSessionData());

			//gameMode->LoadPlayerInventoryData(this);
		}
	}
}

void APW_PlayerController::ClientLoadGameSessionData_Implementation(FGameSessionData GameSessionData)
{
	DEBUG_STRING ("Loaded Game Session Data : Money - " + FString::FromInt(GameSessionData._money) + " Day - " + FString::FromInt(GameSessionData._dayIndex));
	_money = GameSessionData._money;
	_dayIndex = GameSessionData._dayIndex;
	
	TArray<FString> itemIDs = GameSessionData._playersInventoryData.GetInventoryItemIDs(_playerName);
	LoadInventoryItemsByID(itemIDs);
}

void APW_PlayerController::ServerRequestTime_Implementation(float timeOfClientRequest)
{
	float serverTime = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(timeOfClientRequest, serverTime);
}

void APW_PlayerController::ClientReportServerTime_Implementation(float timeOfClientRequest, float serverTime)
{
	float roundTripTime = GetWorld()->GetTimeSeconds() - timeOfClientRequest;
	float currentServerTime = serverTime + (roundTripTime / 2);
	_clientServerDelta = currentServerTime - GetWorld()->GetTimeSeconds();
}
