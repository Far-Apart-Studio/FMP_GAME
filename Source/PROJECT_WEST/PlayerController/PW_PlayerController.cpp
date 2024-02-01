// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_PlayerController.h"

#include "GameFramework/PlayerState.h"
#include "PROJECT_WEST/HUD/PW_HUD.h"
#include "PROJECT_WEST/HUD/PW_CharacterOverlayWidget.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

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

	HandleCheckPing(DeltaTime);
}

void APW_PlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

float APW_PlayerController::GetServerTime()
{
	return  0;
}

void APW_PlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
}

void APW_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void APW_PlayerController::SetHUDHealth(float health, float maxHealth)
{
	_hud = _hud == nullptr ? Cast<APW_HUD>(GetHUD()) : _hud;
	if (_hud && _hud->GetCharacterOverlayWidget())
	{
		_hud->GetCharacterOverlayWidget()->SetHealth(health, maxHealth);
	}
}

void APW_PlayerController::SetHUDScore(float score)
{	_hud = _hud == nullptr ? Cast<APW_HUD>(GetHUD()) : _hud;
	if (_hud && _hud->GetCharacterOverlayWidget())
	{
		_hud->GetCharacterOverlayWidget()->SetScore(score);
	}
}

void APW_PlayerController::StartHighPingWarning()
{
	_hud = _hud == nullptr ? Cast<APW_HUD>(GetHUD()) : _hud;
	if (_hud && _hud->GetCharacterOverlayWidget())
	{
		_hud->GetCharacterOverlayWidget()->StartHighPingWarning();
	}
}

void APW_PlayerController::StopHighPingWarning()
{
	_hud = _hud == nullptr ? Cast<APW_HUD>(GetHUD()) : _hud;
	if (_hud && _hud->GetCharacterOverlayWidget())
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

	if (_hud && _hud->GetCharacterOverlayWidget()->IsHighPingWarningPlaying())
	{
		_pingAnimationRunningTime += DeltaTime;
		if (_pingAnimationRunningTime >= _highPingDuration)
		{
			StopHighPingWarning();
		}
	}
}
