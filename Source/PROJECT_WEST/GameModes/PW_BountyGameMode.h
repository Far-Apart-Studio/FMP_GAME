// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PM_GameMode.h"
#include "PW_BountyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API APW_BountyGameMode : public APM_GameMode
{
	GENERATED_BODY()

public:

	APW_BountyGameMode();
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	virtual void Logout(AController* Exiting) override;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Lobby" )
	FString _mapPath;
};
