// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_ChargeableMechanismActivator.h"

#include "PROJECT_WEST/Items/PW_ChargeableMechanism.h"

UPW_ChargeableMechanismActivator::UPW_ChargeableMechanismActivator()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UPW_ChargeableMechanismActivator::BeginPlay()
{
	Super::BeginPlay();
	InitializeChargeableMechanisms();
}

void UPW_ChargeableMechanismActivator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPW_ChargeableMechanismActivator::InitializeChargeableMechanisms()
{
	for (APW_ChargeableMechanism* chargeableMechanism : _chargeableMechanisList)
	{
		if (chargeableMechanism)
		{
			chargeableMechanism->_OnChargeCompleted.AddDynamic(this, &UPW_ChargeableMechanismActivator::OnChargeCompleted);
		}
	}
}

void UPW_ChargeableMechanismActivator::OnChargeCompleted()
{
	_complctedChargeableMechanisms++;

	if (_complctedChargeableMechanisms == _chargeableMechanisList.Num())
	{
		_OnAllChargeCompleted.Broadcast();
	}
}

