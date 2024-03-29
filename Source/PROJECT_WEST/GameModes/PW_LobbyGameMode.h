// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PW_GameMode.h"
#include "PW_LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API APW_LobbyGameMode : public APW_GameMode
{
	GENERATED_BODY()

protected:
	
	APW_LobbyGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	virtual void Logout(AController* Exiting) override;
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Info" )
	class UBountySystemComponent* _bountySystemComponent;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Gameplay",meta = (AllowPrivateAccess = "true") )
	TSubclassOf<class APW_DebtCollector> _debtCollectorClass;

	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "Info" )
	class APW_BountyBoard* _bountyBoard;

	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "Info" )
	class APW_TransitionPortal* _transitionPortal;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Info")
	APW_DebtCollector* _debtCollector;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Gameplay" , meta = (AllowPrivateAccess = "true") )
	int32 _bountyCollectorDayInterval;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Gameplay",meta = (AllowPrivateAccess = "true") )
	TSubclassOf<class APW_Currency> _currencyClass;
	

	class APW_SpawnPointsManager* _spawnPointsManager;

public:

	void BeginPlay() override;
	void LoadGameSessionData() override;

	UFUNCTION()
	void OnTransitionCompleted();
	
	void TriggerDebtCollector();
	
	void TryPayDebtCollector();
	
	void ResetSessionData();
	
	FORCEINLINE UBountySystemComponent* GetBountySystemComponent() const { return _bountySystemComponent; }
	FORCEINLINE APW_BountyBoard* GetBountyBoard() const { return _bountyBoard; }
	
};
