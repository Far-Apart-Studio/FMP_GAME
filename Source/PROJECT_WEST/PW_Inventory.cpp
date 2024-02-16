// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_Inventory.h"

UPW_Inventory::UPW_Inventory()
{
	PrimaryComponentTick.bCanEverTick = true;
	
}

void UPW_Inventory::BeginPlay()
{
	Super::BeginPlay();
	
}

void UPW_Inventory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

