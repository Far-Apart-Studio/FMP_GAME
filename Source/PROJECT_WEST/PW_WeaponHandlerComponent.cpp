// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_WeaponHandlerComponent.h"

UPW_WeaponHandlerComponent::UPW_WeaponHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPW_WeaponHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UPW_WeaponHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

