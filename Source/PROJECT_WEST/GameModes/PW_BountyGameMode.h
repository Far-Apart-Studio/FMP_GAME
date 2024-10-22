// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PW_GameMode.h"
#include "PROJECT_WEST/Bounty System/PW_BountyData.h"
#include "PROJECT_WEST/SideObjective/PW_SideObjective.h"
#include "PW_BountyGameMode.generated.h"

namespace MatchState
{
	extern PROJECT_WEST_API const FName Lose;
	extern PROJECT_WEST_API const FName Win;
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
	void Tick(float DeltaSeconds) override;
	
	void PostLogin(APlayerController* NewPlayer) override;
	void Logout(AController* Exiting) override;

	void PlayerEliminated(class APW_Character* ElimmedCharacter, class APW_PlayerController* VictimController, AController* AttackerController) override;

	float _matchStartTime;
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Gameplay" )
	FMapDataEntry _lobbyMapData;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Gameplay" )
	TSubclassOf<class APW_Lantern > _lanternClass;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Gameplay" )
	TSubclassOf<AActor > _bountyEnemyClass;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Gameplay" )
	TSubclassOf<AActor > _bountyPortalClass;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Gameplay" )
	TSubclassOf<AActor > _bountyHeadClass;
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Gameplay" )
	TSubclassOf<class APW_ExtractionPoint > _extractionPointClass;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Gameplay" )
	TSubclassOf<AActor > _enemyClass;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Gameplay",meta = (AllowPrivateAccess = "true") )
	TSubclassOf<class APW_Currency> _currencyClass;
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Gameplay" )
	TSubclassOf<class APW_AutoEnemySpawner > _autoSpawnerClass;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Gameplay",meta = (AllowPrivateAccess = "true") )
	int32 _numOfCurrencies;

	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay",meta = (AllowPrivateAccess = "true") )
	APW_Lantern* _lantern;

	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay",meta = (AllowPrivateAccess = "true") )
	class APW_SideObjectiveManager* _sideObjectiveManager;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay",meta = (AllowPrivateAccess = "true") )
	TMap<APW_PlayerController*, APW_PlayerController*> _spectatingPlayersMap;

	AActor* _bountyPortalEntrance;

	AActor* _bountyEnemy;

	AActor* _bountyHead;

	APW_ExtractionPoint* _extractionPoint;

	UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay")
	void OnInitializeComplete();

	void OnTimeUp();
	
	void BountySuccessful();
	
	void LoadGameSessionData() override;
	
	void SpawnLantern();

	void SpawnBountyPortal();

	void SpawnBountyEnemy();
	UFUNCTION()
	void OnBountyDead(AActor* OwnerActor, AActor* DamageCauser, AController* DamageCauserController);
	void SpawnBountyHead();
	
	UFUNCTION()
	void OnActivateExtrationPoint(bool bWinCondition,TArray<FString> escapedPlayers);

	UFUNCTION()
	void OnPlayerTriggerExtractionPoint(APW_Character* Player);
	void SpawnExtractionPoint();
	
	void SpawnAutoEnemySpawner(ACharacter *character);

	UFUNCTION()
	void EnemyEliminated(AActor* OwnerActor,AActor* DamageCauser, AController* DamageCauserController);

	UFUNCTION()
	void OnSideObjectiveCompleted(APW_SideObjective* ComplectedObjective);

	void SpawnCurrencies();

	int _enemyCount = 0;
	int _numOfenemiesPerPoint = 3;

	FORCEINLINE float GetMatchTime() const { return _matchTime; }
	FORCEINLINE float GetEndMatchCooldownTime() const { return _mathEndCooldownTime; }
	FORCEINLINE float GetLevelStartTime() const { return _levelStartTime; }
	FORCEINLINE float GetCountdownTime() const { return _countdownTime; }
	
protected:

	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true") )
	class APW_SpawnPointsManager* _spawnPointsManager;

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
	void UpdateDeadSpecators(class APW_PlayerController* PlayerController);
	APW_PlayerController* GetAnyPlayerAlive();
};
