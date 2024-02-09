// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_LobbyGameMode.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "PROJECT_WEST/Bounty System/BountySystemComponent.h"
#include "PROJECT_WEST/Bounty System/PW_BountyBoard.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/Gameplay/PW_GameInstance.h"

APW_LobbyGameMode::APW_LobbyGameMode()
{
	bUseSeamlessTravel = true;
	_mapPath = "";
	_bountySystemComponent = CreateDefaultSubobject<UBountySystemComponent>( "BountySystemComponent" );
}

void APW_LobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	_bountyBoard = Cast<APW_BountyBoard>(UGameplayStatics::GetActorOfClass(GetWorld(), APW_BountyBoard::StaticClass()));
	if ( _bountyBoard)
	{
		DEBUG_STRING("Bounty Board Found");
	}
	
	ToggleSessionLock(false);
}