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
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Lobby" )
	FString _mapPath;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Lobby" )
	class UBountySystemComponent* _bountySystemComponent;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Lobby" )
	TSubclassOf<class APW_DebtCollector> _debtCollectorClass;

	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "Lobby" )
	class APW_BountyBoard* _bountyBoard;

	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "Lobby" )
	class APW_TransitionPortal* _transitionPortal;

	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "Lobby" )
	APW_DebtCollector* _debtCollector;

	class APW_SpawnPointsManager* _spawnPointsManager;
	class UPW_SpawnPointsHandlerComponent* _spawnPointsHandlerComponent;

public:

	void BeginPlay() override;
	void LoadGameSessionData() override;

	UFUNCTION()
	void OnTransitionCompleted();

	void TriggerDebtCollector();
	
	FORCEINLINE UBountySystemComponent* GetBountySystemComponent() const { return _bountySystemComponent; }
	FORCEINLINE APW_BountyBoard* GetBountyBoard() const { return _bountyBoard; }
};
