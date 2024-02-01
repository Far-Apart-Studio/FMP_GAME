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
	
	virtual void PlayerEliminated(class APW_Character* ElimmedCharacter, class APW_PlayerController* VictimController, APlayerController* AttackerController);
	
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);

	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage);

	void PlayerLeftGame(class APW_PlayerState* PlayerLeaving);

	void ServerTravel(FString MapPath);
	
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
