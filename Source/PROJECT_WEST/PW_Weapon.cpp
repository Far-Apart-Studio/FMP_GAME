// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_Weapon.h"

#include "PW_WeaponData.h"

APW_Weapon::APW_Weapon()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APW_Weapon::BeginPlay()
{
	Super::BeginPlay();
}

void APW_Weapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APW_Weapon::InitialiseWeapon()
{
	_currentAmmo = _weaponData->GetWeaponMagazineCapacity();
}
