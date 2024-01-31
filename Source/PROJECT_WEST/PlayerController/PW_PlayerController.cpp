// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_PlayerController.h"
#include "PROJECT_WEST/HUD/PW_HUD.h"
#include "PROJECT_WEST/HUD/PW_CharacterOverlayWidget.h"

void APW_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void APW_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	_hud = Cast<APW_HUD>(GetHUD());
}

void APW_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
