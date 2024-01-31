// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PW_LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API APW_LobbyGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	
	APW_LobbyGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	virtual void Logout(AController* Exiting) override;

	UFUNCTION(BlueprintCallable)
	void ServerTravel (FString mapPath);

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Lobby" )
	FString _mapPath;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Lobby" )
	int32 _playersNeeded;
};
