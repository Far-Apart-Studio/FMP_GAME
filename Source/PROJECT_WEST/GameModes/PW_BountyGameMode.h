// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PW_GameMode.h"
#include "PW_BountyGameMode.generated.h"

namespace MatchState
{
	extern PROJECT_WEST_API const FName Cooldown; // cooldown before match ends
}

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

	virtual void PlayerEliminated(class APW_Character* ElimmedCharacter, class APW_PlayerController* VictimController, AController* AttackerController) override;
	void EnemyEliminated(APW_Character* AttackerCharacter, APW_PlayerController* AttackerController);

	void BountySuccessful();

	float _matchStartTime;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Gameplay" )
	FString _mapPath;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Gameplay" )
	TSubclassOf<class APW_Lantern > _lanternClass;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Gameplay" )
	TSubclassOf<AActor > _bountyEnemyClass;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Gameplay" )
	TSubclassOf<AActor > _bountyHeadClass;

	
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Gameplay" )
	TSubclassOf<class APW_ExtractionPoint > _extractionPointClass;

	APW_Lantern* _lantern;

	AActor* _bountyEnemy;

	AActor* _bountyHead;

	APW_ExtractionPoint* _extractionPoint;

	void SpawnLantern();
	
	UFUNCTION()
	void OnBountyDead(AActor* DamageCauser, AController* DamageCauserController);
	void SpawnBountyEnemy();
	void SpawnBountyHead();
	UFUNCTION()
	void OnActivateExtrationPoint(bool bWinCondition);
	void SpawnExtractionPoint();

	FORCEINLINE float GetMatchTime() const { return _matchTime; }
	FORCEINLINE float GetEndMatchCooldownTime() const { return _mathEndCooldownTime; }
	FORCEINLINE float GetLevelStartTime() const { return _levelStartTime; }
	FORCEINLINE float GetCountdownTime() const { return _countdownTime; }
	
protected:
	
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

	class APW_SpawnPointsManager* _spawnPointsManager;
	class UPW_SpawnPointsHandlerComponent* _spawnPointsHandlerComponent;

	void ToggleAllPlayersInput(bool bEnable);

	float _countdownTime = 0.f;

	float _levelStartTime;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	float _matchTime;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	float _mathEndCooldownTime;

	bool _bountySuccessful = false;
	
private:

	float _countDownTime;

	void HandleStateTimer();
	void BountyFailed();
	APW_PlayerController* GetAnyPlayerAlive();
};
