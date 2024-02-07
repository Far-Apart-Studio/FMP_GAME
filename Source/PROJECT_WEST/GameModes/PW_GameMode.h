// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PW_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API APW_GameMode : public AGameMode
{
	GENERATED_BODY()

public:
	APW_GameMode();
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	virtual void Logout(AController* Exiting) override;

	virtual void Tick(float DeltaTime) override;
	
	virtual void PlayerEliminated(class APW_Character* ElimmedCharacter, class APW_PlayerController* VictimController, AController* AttackerController);

	void PlayerLeftGame(class APW_PlayerState* PlayerLeaving);

	void ToggleSessionLock(bool lock);

	UFUNCTION(BlueprintCallable)
	void ServerTravel (FString mapPath);
	
protected:
	
	virtual void BeginPlay() override;
	
	virtual void OnMatchStateSet() override;
};
