// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_PlayerState.h"
#include "PROJECT_WEST/PW_CharacterController.h"
#include "PROJECT_WEST/PlayerController/PW_PlayerController.h"


void APW_PlayerState::AddToScore(int32 scoreAmount)
{
	SetScore( GetScore() + scoreAmount );
	_character  = _character == nullptr ? Cast<APW_CharacterController>(GetPawn()) : _character;

	if (_character != nullptr)
	{
		_playerController = _playerController == nullptr ? Cast<APW_PlayerController>(_character->Controller) : _playerController;
		if (_playerController != nullptr)
		{
			_playerController->SetHUDScore(GetScore());
		}
	}
}

void APW_PlayerState::OnRep_Score()
{
	Super::OnRep_Score();
	_character  = _character == nullptr ? Cast<APW_CharacterController>(GetPawn()) : _character;

	if (_character != nullptr)
	{
		_playerController = _playerController == nullptr ? Cast<APW_PlayerController>(_character->Controller) : _playerController;
		if (_playerController != nullptr)
		{
			_playerController->SetHUDScore(GetScore());
		}
	}
}