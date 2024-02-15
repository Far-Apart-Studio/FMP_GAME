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
#include "Kismet/KismetMathLibrary.h"
#include "PROJECT_WEST/GameModes/PW_GameMode.h"
#include "PROJECT_WEST/HUD/PW_AnnouncementWidget.h"

APW_PlayerController::APW_PlayerController()
{
	_matchTime = 0.f;
	_clientServerDelta = 0;
	_timeSyncFrequency = 5;
	_timeSyncRuningTime = 0;

	//DEBUG_STRING( "APW_PlayerController is created" );
}

void APW_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// called when the player controller possesses a pawn
	_hud = Cast<APW_HUD>(GetHUD());
	_votedBountyIndex = -1;
	_hasVoted = false;
}

void APW_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	//DEBUG_STRING( "APW_PlayerController BeginPlay" );

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

bool APW_PlayerController::IsAlive()
{
	APW_Character* character = Cast<APW_Character>(GetPawn());
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

void APW_PlayerController::Destroyed()
{
	// Drop all items when player controller is destroyed
	if (_hasVoted)
	{
		RemoveVoteFromBounty(_votedBountyIndex);
	}
	DropAllItems();
	Super::Destroyed();
}

void APW_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void APW_PlayerController::SetHUDHealth(float health, float maxHealth)
{

}

void APW_PlayerController::SetHUDScore(float score)
{

}

void APW_PlayerController::SetMatchCountdown(float time)
{
	if (_hud && _hud->GetCharacterOverlayWidget())
	{
		if (time < 0)
		{
			time = 0;
			_hud->GetCharacterOverlayWidget()->SetTimeText("00:00");
		}
		
		_hud->GetCharacterOverlayWidget()->SetTimeText(ConvertToTime(time));
		//DEBUG_STRING ( ConvertToTime(time) );
	}
	else
	{
		DEBUG_STRING( "HUD is not valid" );
	}
}

void APW_PlayerController::SetMatchEndCountdown(float time)
{
	if (_hud && _hud->GetCharacterOverlayWidget())
	{
		if (time < 0)
		{
			time = 0;
			_hud->GetCharacterOverlayWidget()->SetTimeText(ConvertToTime(time));
		}

		_hud->GetCharacterOverlayWidget()->SetTimeText(ConvertToTime(time));
		//DEBUG_STRING( ConvertToTime(time) );
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
	
	if(_hud)
	{
		_hud->DisplayAccouncement("Match Ended, Returning to Lobby", FColor::Red, _endMatchCountdown);
	}
	
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

bool APW_PlayerController::IsHUDValid()
{
	_hud = _hud == nullptr ? Cast<APW_HUD>(GetHUD()) : _hud;
	return _hud != nullptr;
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
	if (_hud && _hud->GetCharacterOverlayWidget())
	{
		_hud->GetCharacterOverlayWidget()->StartHighPingWarning();
	}
}

void APW_PlayerController::StopHighPingWarning()
{
	if (_hud && _hud->GetCharacterOverlayWidget())
	{
		_hud->GetCharacterOverlayWidget()->StopHighPingWarning();
	}
}

void APW_PlayerController::HandleCheckPing(float DeltaTime)
{
	if(!IsHUDValid()) return;
	
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
	
	if (_hud &&
		 _hud->GetCharacterOverlayWidget() &&
		 _hud->GetCharacterOverlayWidget()->IsHighPingWarningPlaying())
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
