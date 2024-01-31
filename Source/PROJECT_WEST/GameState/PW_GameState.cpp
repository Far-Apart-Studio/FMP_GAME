// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_GameState.h"
#include "Net/UnrealNetwork.h"
#include "PROJECT_WEST/PlayerState/PW_PlayerState.h"
#include "DrawDebugHelpers.h"
#include "PROJECT_WEST/DebugMacros.h"

APW_GameState::APW_GameState()
{
	_topScore = 0.0f;
}

void APW_GameState::BeginPlay()
{
	Super::BeginPlay();

	//PrintString("GameState Constructor");
}

void APW_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME( APW_GameState, _topScoringPlayers);
}

void APW_GameState::UpdateTopScore(APW_PlayerState* scoringPlayer)
{
	if (_topScoringPlayers.Num() == 0)
	{
		_topScoringPlayers.Add(scoringPlayer);
		_topScore = scoringPlayer->GetScore();
	}
	else if (scoringPlayer->GetScore() == _topScore)
	{
		_topScoringPlayers.AddUnique(scoringPlayer);
	}
	else if (scoringPlayer->GetScore() > _topScore)
	{
		_topScoringPlayers.Empty();
		_topScoringPlayers.Add(scoringPlayer);
		_topScore = scoringPlayer->GetScore();
	}
}
