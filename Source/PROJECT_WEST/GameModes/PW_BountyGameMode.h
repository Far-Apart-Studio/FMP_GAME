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
	virtual void Tick(float DeltaSeconds) override;
	
	void PostLogin(APlayerController* NewPlayer) override;
	void Logout(AController* Exiting) override;
	
	void EnemyEliminated(APW_Character* AttackerCharacter, APW_PlayerController* AttackerController);

	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, Category = "Game" )
	float _warmUpTime;

	float _matchStartTime;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Game" )
	FString _mapPath;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Game" )
	TSubclassOf<class APW_Lantern > _lanternClass;

	APW_Lantern* _lantern;

	void SpawnLantern();
protected:
	
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

	class APW_SpawnPointsManager* _spawnPointsManager;
	class UPW_SpawnPointsHandlerComponent* _spawnPointsHandlerComponent;
	
private:

	float _countDownTime;
};
