// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_GameState.h"
#include "Net/UnrealNetwork.h"
#include "PROJECT_WEST/PlayerState/PW_PlayerState.h"
#include "DrawDebugHelpers.h"
#include "PROJECT_WEST/PlayerController/PW_PlayerController.h"
#include "PROJECT_WEST/DebugMacros.h"

APW_GameState::APW_GameState()
{

}

void APW_GameState::BeginPlay()
{
	Super::BeginPlay();

	DEBUG_STRING("GameState BeginPlay");
}

void APW_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
}

APW_PlayerController* APW_GameState::GetLocalPlayerController()
{
	APW_PlayerController* localPlayer = nullptr;
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APW_PlayerController* playerController = Cast<APW_PlayerController>(*Iterator);
		if (playerController && playerController->IsLocalController())
		{
			localPlayer = playerController;
			break;
		}
	}
	return localPlayer;
}
