// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_DebtCollector.h"

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
	_debtAmount = _debtStartAmount + ((day - 1) * _debtIncreaseAmount);
}

