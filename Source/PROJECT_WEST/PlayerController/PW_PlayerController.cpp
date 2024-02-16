// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_PlayerController.h"
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

	// called Only on Host when player is spawned

	//DEBUG_STRING( "APW_PlayerController OnPossess" );
	
	_votedBountyIndex = -1;
	_hasVoted = false;
}

void APW_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	// called only once when player is spawned
	//DEBUG_STRING ( "APW_PlayerController BeginPlay" );

	GetCharacterOverlayWidget();
	
	ServerCheckMatchState();
	
	_highPingRunningTime = 0;
	_highPingDuration = 5;
	_checkPingFrequency = 20;
	_highPingThreshold = 50;

	LoadGameSessionData();
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

	HandleCheckPing(DeltaTime);
}

void APW_PlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME( APW_PlayerController, _matchState );
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

bool APW_PlayerController::IsAlive()
{
	const APW_Character* character = Cast<APW_Character>(GetPawn());
	if (character)
	{
		return character->IsAlive();
	}
	return false;
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
	if (!_announcementWidget)
	{
		_announcementWidget = CreateWidget<UPW_AnnouncementWidget>(this, _announcementWidgetClass);
	}

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
	if(_characterOverlayWidget)
	{
		_characterOverlayWidget->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void APW_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}


UPW_CharacterOverlayWidget*  APW_PlayerController::GetCharacterOverlayWidget()
{
	if (!_characterOverlayWidget)
	{
		_characterOverlayWidget = CreateWidget<UPW_CharacterOverlayWidget>(this, _characterOverlayWidgetClass);
		if (_characterOverlayWidget != nullptr)
		{
			DEBUG_STRING( "CharacterOverlayWidget Added" );
			_characterOverlayWidget->AddToViewport();
		}
	}
	return _characterOverlayWidget;
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
		}
		
		_characterOverlayWidget->SetTimeText(ConvertToTime(time));
		DEBUG_STRING ( ConvertToTime(time) );
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
		}

		_characterOverlayWidget->SetTimeText(ConvertToTime(time));
		DEBUG_STRING( ConvertToTime(time) );
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

void APW_PlayerController::StartHighPingWarning()
{
	if (GetCharacterOverlayWidget())
	{
		_characterOverlayWidget->StartHighPingWarning();
	}
}

void APW_PlayerController::StopHighPingWarning()
{
	if (GetCharacterOverlayWidget())
	{
		_characterOverlayWidget->StopHighPingWarning();
	}
}

void APW_PlayerController::HandleCheckPing(float DeltaTime)
{
	if(!GetCharacterOverlayWidget()) return;
	
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
	
	if (_characterOverlayWidget &&
		_characterOverlayWidget->IsHighPingWarningPlaying())
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
	_lobbyGameMode == nullptr ? _lobbyGameMode = Cast<APW_LobbyGameMode>(UGameplayStatics::GetGameMode(this)) : nullptr;
	if (_lobbyGameMode)
	{
		FBountyDataEntry bounty = _lobbyGameMode->GetBountyBoard()->GetBountyDataList()[bountyIndex];
		if (bounty._bountyCost <= _lobbyGameMode->_gameInstance->GetGameSessionData()._money)
		{
			_lobbyGameMode->GetBountyBoard()->AddVoteToBounty(bountyIndex);
			return true;
		}
	}
	return false;
}

bool APW_PlayerController::LocalRemoveVoteFromBounty(int32 bountyIndex)
{
	_lobbyGameMode == nullptr ? _lobbyGameMode = Cast<APW_LobbyGameMode>(UGameplayStatics::GetGameMode(this)) : nullptr;
	if (_lobbyGameMode)
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
	DEBUG_STRING( "ServerClearVote" );
	
	if (HasAuthority())
	{
		_lobbyGameMode == nullptr ? _lobbyGameMode = Cast<APW_LobbyGameMode>(UGameplayStatics::GetGameMode(this)) : nullptr;
		if (_lobbyGameMode)
		{
			DEBUG_STRING( "ServerClearVote Done : " + FString::FromInt(_votedBountyIndex) );
			_lobbyGameMode->GetBountyBoard()->RemoveVoteFromBounty(_votedBountyIndex);
		}
		else
		{
			DEBUG_STRING( "ServerClearVote: No LobbyGameMode" );
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
	if(HasAuthority()) return;
	SeverLoadGameSessionData();
}

void APW_PlayerController::SeverLoadGameSessionData_Implementation()
{
	if(HasAuthority())
	{
		APW_GameMode* gameMode = Cast<APW_GameMode>(UGameplayStatics::GetGameMode(this));
		if (gameMode)
		{
			ClientLoadGameSessionData(gameMode->_gameInstance->GetGameSessionData());
		}
	}
}

void APW_PlayerController::ClientLoadGameSessionData_Implementation(FGameSessionData GameSessionData)
{
	_money = GameSessionData._money;
	_dayIndex = GameSessionData._dayIndex;
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
