// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_GameInstance.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/Bounty System/PW_BountyData.h"

UPW_GameInstance::UPW_GameInstance()
{
	
}

void UPW_GameInstance::Init()
{
	Super::Init();
	//DEBUG_STRING("GameInstance Init");
	_gameSessionData = FGameSessionData();
	_gameSessionData._bountyDataEntry._bountyCost = 100;
	_gameSessionData._bountyDataEntry._bountyReward = 1000;

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UPW_GameInstance::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UPW_GameInstance::EndLoadingScreen);
}

void UPW_GameInstance::BeginLoadingScreen(const FString& MapName)
{
	_OnLoadingStart.Broadcast();
}

void UPW_GameInstance::EndLoadingScreen(UWorld* InLoadedWorld)
{
	_OnLoadingEnd.Broadcast();
}

void UPW_GameInstance::OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld)
{
	Super::OnWorldChanged(OldWorld, NewWorld);
	FString OldWorldName = OldWorld ? OldWorld->GetName() : "nullptr";
	FString NewWorldName = NewWorld ? NewWorld->GetName() : "nullptr";
	//DEBUG_STRING("GameInstance World Changed from " + OldWorldName + " to " + NewWorldName);
}

void UPW_GameInstance::Shutdown()
{
	//DEBUG_STRING("GameInstance Shutdown");
	Super::Shutdown();
}
