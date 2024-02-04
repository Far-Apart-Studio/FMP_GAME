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
#include "Kismet/KismetMathLibrary.h"

APW_PlayerController::APW_PlayerController()
{
	_matchTime = 120.f;
	_clientServerDelta = 0;
	_timeSyncFrequency = 5;
	_timeSyncRuningTime = 0;
	_InGameplaySession = false;
}

void APW_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void APW_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	_hud = Cast<APW_HUD>(GetHUD());
	_highPingRunningTime = 0;
	_highPingDuration = 5;
	_checkPingFrequency = 20;
	_highPingThreshold = 50;
}

void APW_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(_InGameplaySession)
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

void APW_PlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		ServerRequestTime(GetWorld()->GetTimeSeconds());
	}
}

void APW_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void APW_PlayerController::SetHUDHealth(float health, float maxHealth)
{
	if (IsHUDValid() && _hud->GetCharacterOverlayWidget())
	{
		//_hud->GetCharacterOverlayWidget()->SetHealth(health, maxHealth);
	}
}

void APW_PlayerController::SetHUDScore(float score)
{
	if (IsHUDValid() && _hud->GetCharacterOverlayWidget())
	{
		//_hud->GetCharacterOverlayWidget()->SetScore(score);
	}
}

void APW_PlayerController::SetMatchCountdown(float time)
{
	if (IsHUDValid() && _hud->GetCharacterOverlayWidget())
	{
		int32 minutes = FMath::FloorToInt(time / 60);
		int32 seconds = time - (minutes * 60);
		FString timeString = FString::Printf(TEXT("%02d:%02d"), minutes, seconds);

		DEBUG_STRING( timeString);
		
		//_hud->GetCharacterOverlayWidget()->SetScore(score);
	}
}

void APW_PlayerController::OnMatchStateSet(FName matchState) // Ran Only on Server
{
	_matchState = matchState;
	OnMatchStateChanged(_matchState);
}

void APW_PlayerController::OnRep_MatchState() // Ran Only on Clients
{
	OnMatchStateChanged(_matchState);
}

void APW_PlayerController::OnMatchStateChanged(FName matchState)
{
	if (_matchState == MatchState::InProgress)
	{
		_InGameplaySession = true;
	}
	else if (_matchState == MatchState::LeavingMap)
	{
		_InGameplaySession = false;
	}
}

bool APW_PlayerController::IsHUDValid()
{
	_hud = _hud == nullptr ? Cast<APW_HUD>(GetHUD()) : _hud;
	return _hud != nullptr;
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

void APW_PlayerController::StartHighPingWarning()
{
	if (IsHUDValid() && _hud->GetCharacterOverlayWidget())
	{
		_hud->GetCharacterOverlayWidget()->StartHighPingWarning();
	}
}

void APW_PlayerController::StopHighPingWarning()
{
	if (IsHUDValid() && _hud->GetCharacterOverlayWidget())
	{
		_hud->GetCharacterOverlayWidget()->StopHighPingWarning();
	}
}

void APW_PlayerController::HandleCheckPing(float DeltaTime)
{
	_highPingRunningTime += DeltaTime;
	if (_highPingRunningTime >= _checkPingFrequency)
	{
		PlayerState = PlayerState == nullptr ? GetPlayerState<APlayerState>() : PlayerState;
		if (PlayerState)
		{
			if (PlayerState->GetPing() * 4 >= _highPingThreshold) //  ping is compressed, so we need to multiply it by 4
			{
				StartHighPingWarning();
				_pingAnimationRunningTime = 0;
			}
		}
		_highPingRunningTime = 0;
	}

	if (IsHUDValid() && _hud->GetCharacterOverlayWidget() && _hud->GetCharacterOverlayWidget()->IsHighPingWarningPlaying())
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
	uint32 secondsLeft = FMath::CeilToInt(_matchTime  - GetServerTime());

	if (HasAuthority())
	{
		//if (BlasterGameMode == nullptr)
		//{
		//	BlasterGameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
		//	LevelStartingTime = BlasterGameMode->LevelStartingTime;
		//}
		//BlasterGameMode = BlasterGameMode == nullptr ? Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)) : BlasterGameMode;
		//if (BlasterGameMode)
		//{
		//	SecondsLeft = FMath::CeilToInt(BlasterGameMode->GetCountdownTime() + LevelStartingTime);
		//}
	}

	
	if (_countDownInt != secondsLeft)
	{
		_countDownInt = secondsLeft;
		SetMatchCountdown(secondsLeft);
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

void APW_PlayerController::PawnLeavingGame()
{
	APW_Character* character = Cast<APW_Character>(GetPawn());
	if (character)
	{
		//character->Elim(true);
		//character->ServerLeaveGame();
	}

	DEBUG_STRING( "PawnLeavingGame");

	Super::PawnLeavingGame();
}
