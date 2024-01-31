// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PW_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API APW_PlayerState : public APlayerState
{
	GENERATED_BODY()
public:

	// Score
	virtual void OnRep_Score() override;
	void AddToScore(int32 scoreAmount);

private:
	class APW_CharacterController* _character;
	class APW_PlayerController* _playerController;
};
