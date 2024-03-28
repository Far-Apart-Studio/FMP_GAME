// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_PlayerState.h"

#include "Net/UnrealNetwork.h"
#include "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/PlayerController/PW_PlayerController.h"
#include "PROJECT_WEST/DebugMacros.h"


APW_PlayerState::APW_PlayerState()
{
	
}

void APW_PlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	DEBUG_STRING("PW_PlayerState BeginPlay");
	
	//_character  = _character == nullptr ? Cast<APW_Character>(GetPawn()) : _character;
	//_playerController = _playerController == nullptr ? Cast<APW_PlayerController>(_character->Controller) : _playerController;
}

void APW_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME( APW_PlayerState, _colorIndex);
}

void APW_PlayerState::ClientSetColorIndex_Implementation(int index)
{
	
}
