// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PW_GameMode.h"
#include "PW_BountyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API APW_BountyGameMode : public APW_GameMode
{
	GENERATED_BODY()

public:

	APW_BountyGameMode();
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	virtual void Logout(AController* Exiting) override;

	void EnemyEliminated(APW_CharacterController* AttackerCharacter, APW_PlayerController* AttackerController);
	

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Lobby" )
	FString _mapPath;
};
