// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PM_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API APM_GameMode : public AGameMode
{
	GENERATED_BODY()

public:
	APM_GameMode();
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	void ServerTravel(FString MapPath);
	virtual void Tick(float DeltaTime) override;
	
	virtual void PlayerEliminated(class APW_CharacterController* ElimmedCharacter, class APW_PlayerController* VictimController, APlayerController* AttackerController);
	
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);
	void PlayerLeftGame(class APW_PlayerState* PlayerLeaving);
	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage);
	
protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;
	
	float CountdownTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	float WarmupTime = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	float MatchTime = 120.f;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	float CooldownTime = 10.f;
	
	float LevelStartingTime = 0.f;

	bool bTeamsMatch = false;
	
public:
	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }
};
