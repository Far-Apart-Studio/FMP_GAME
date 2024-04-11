// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_DebtCollector.h"

#include "Components/PW_HighlightComponent.h"
#include "Net/UnrealNetwork.h"
#include  "PROJECT_WEST/GameModes/PW_GameMode.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/GameModes/PW_LobbyGameMode.h"
#include "PROJECT_WEST/PlayerController/PW_PlayerController.h"

APW_DebtCollector::APW_DebtCollector()
{
	PrimaryActorTick.bCanEverTick = true;

	_debtMinStartAmount = 100;
	_debtMaxStartAmount = 500;

	_debtMinIncreaseAmount = 100;
	_debtMaxIncreaseAmount = 500;
	_hasInteracted = false;
	_debtIncreaseValue = 100;
}

void APW_DebtCollector::BeginPlay()
{
	Super::BeginPlay();
	SetReplicates(true);
	SetReplicateMovement(true);

	_highlightComponent = FindComponentByClass<UPW_HighlightComponent>();
}

void APW_DebtCollector::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APW_DebtCollector, _hasInteracted);
	DOREPLIFETIME(APW_DebtCollector, _debtAmount);
}

void APW_DebtCollector::StartFocus_Implementation(AActor* owner)
{
	if(_highlightComponent)
	{
		_highlightComponent->ShowHighlight();
	}
}

void APW_DebtCollector::EndFocus_Implementation(AActor* owner)
{
	if(_highlightComponent)
	{
		_highlightComponent->HideHighlight();
	}
}

void APW_DebtCollector::StartInteract_Implementation(AActor* owner)
{

}

bool APW_DebtCollector::HasServerInteraction_Implementation()
{
	return true;
}

void APW_DebtCollector::ServerStartInteract_Implementation(AActor* owner)
{
	HandleBounty(owner);
}

void APW_DebtCollector::OnRep_DebtChanged()
{
	OnDebtAmountChanged(_debtAmount);
}

void APW_DebtCollector::OnRep_OnInteractChanged()
{
	FinishedInteraction ();
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
	const int32 randomAmount = FMath::RandRange(_debtMinStartAmount, _debtMaxStartAmount);
	const int32 randomIncrease = FMath::RandRange(_debtMinIncreaseAmount, _debtMaxIncreaseAmount);
	const int32 dayInterval = day % 4;
	
	_debtAmount = randomAmount + ((dayInterval - 1 ) * randomIncrease);
	//_debtAmount = randomAmount + ((dayInterval) * randomIncrease);
	_debtMinIncreaseAmount += _debtIncreaseValue;
	_debtMaxIncreaseAmount += _debtIncreaseValue;
	OnDebtAmountChanged(_debtAmount);
}

void APW_DebtCollector::HandleBounty(AActor* owner)
{
	if (_hasInteracted) return;
	_hasInteracted = true;
	FinishedInteraction ();
	if (APW_LobbyGameMode* lobbyGameMode = GetWorld()->GetAuthGameMode<APW_LobbyGameMode>())
	{
		lobbyGameMode->TryPayDebtCollector();
	}
}
