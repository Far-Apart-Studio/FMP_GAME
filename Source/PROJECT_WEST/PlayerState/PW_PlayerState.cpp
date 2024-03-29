// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_PlayerState.h"

#include "Net/UnrealNetwork.h"
#include "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/PlayerController/PW_PlayerController.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/Gameplay/PW_GameInstance.h"


APW_PlayerState::APW_PlayerState()
{
	bReplicates = true;

	// bind OnPawnSet event
	OnPawnSet.AddDynamic(this, &APW_PlayerState::PawnSet);
}

void APW_PlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	//_character  = _character == nullptr ? Cast<APW_Character>(GetPawn()) : _character;
	//_playerController = _playerController == nullptr ? Cast<APW_PlayerController>(_character->Controller) : _playerController;

	if(UPW_GameInstance* gameInstance = Cast<UPW_GameInstance>(GetGameInstance()))
	{
		//_colorIndex = gameInstance->GetColorIndex();
	}

	//DEBUG_STRING("PW_PlayerState BeginPlay : " + FString::FromInt(_colorIndex));
}

void APW_PlayerState::PawnSet(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	//DEBUG_STRING("PawnSet");
}

void APW_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME( APW_PlayerState, _colorIndex);
}

void APW_PlayerState::SetColorIndex(int32 index)
{
	_colorIndex = index;
}
