// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "PW_GameState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API APW_GameState : public AGameState
{
	GENERATED_BODY()

public:
	
	//TArray<class UPW_BountyData*> _bountyDataList;

	APW_GameState();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(class APW_PlayerState* scoringPlayer);

	UPROPERTY( Replicated)
	TArray<class APW_PlayerState*> _topScoringPlayers;

	float _topScore;
};
