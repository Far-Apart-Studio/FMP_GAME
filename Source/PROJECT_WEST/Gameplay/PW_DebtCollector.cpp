// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_DebtCollector.h"
#include  "PROJECT_WEST/GameModes/PW_GameMode.h"
#include "PROJECT_WEST/DebugMacros.h"

APW_DebtCollector::APW_DebtCollector()
{
	PrimaryActorTick.bCanEverTick = true;

}

void APW_DebtCollector::BeginPlay()
{
	Super::BeginPlay();
	SetReplicates(true);
	SetReplicateMovement(true);
}

void APW_DebtCollector::StartFocus_Implementation()
{
	DEBUG_STRING("StartFocus_Implementation");
}

void APW_DebtCollector::EndFocus_Implementation()
{
	DEBUG_STRING("EndFocus_Implementation");
}

void APW_DebtCollector::StartInteract_Implementation(AActor* owner)
{
	HandleBounty();
}

void APW_DebtCollector::EndInteract_Implementation()
{

}

bool APW_DebtCollector::IsInteracting_Implementation()
{
	return false;
}

void APW_DebtCollector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APW_DebtCollector::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APW_DebtCollector::SetDebtAmount(int32 day)
{
	int32 randomAmount = FMath::RandRange(_debtMinStartAmount, __debtMaxStartAmount); 
	_debtAmount = randomAmount + ((day - 1) * _debtIncreaseAmount);
}

void APW_DebtCollector::HandleBounty()
{
	if (HasAuthority())
	{
		LocalHandleBounty();
	}
	else
	{
		ServerHandleBounty();
	}
}

void APW_DebtCollector::ServerHandleBounty_Implementation()
{
	if (!HasAuthority()) return;
	LocalHandleBounty();
}

void APW_DebtCollector::LocalHandleBounty()
{
	APW_GameMode* gameMode = GetWorld()->GetAuthGameMode<APW_GameMode>();
	if (gameMode)
	{
		int32 avaliableMoney =  gameMode->GetMoney();
		if (avaliableMoney >= _debtAmount)
		{
			gameMode->RemoveMoney(_debtAmount);
			_onInteract.Broadcast(true);
		}
		else
		{
			_onInteract.Broadcast(false);
		}
	}
}

