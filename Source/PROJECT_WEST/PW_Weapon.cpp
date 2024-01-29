// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_Weapon.h"

#include "PW_Utilities.h"
#include "PW_WeaponData.h"

APW_Weapon::APW_Weapon()
{
	PrimaryActorTick.bCanEverTick = true;

	_weaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	_weaponMesh->SetupAttachment(RootComponent);
}

void APW_Weapon::BeginPlay()
{
	Super::BeginPlay();
}

void APW_Weapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APW_Weapon::TransferReserveAmmo()
{
	if (_currentReserveAmmo <= 0)
		return;

	const int ammoRequired = _weaponData->GetWeaponMagazineCapacity() - _currentAmmo;
	const int ammoToTransfer = FMath::Min(ammoRequired, _currentReserveAmmo);
	
	_currentAmmo += ammoToTransfer;
	_currentReserveAmmo -= ammoToTransfer;
}

void APW_Weapon::InitialiseWeapon(UPW_WeaponData* weaponData)
{
	_weaponData = weaponData;
	
	if (_weaponData == nullptr)
		{ PW_Utilities::Log("FAILED TO LOAD WEAPON DATA"); return; }
	
	_currentAmmo = _weaponData->GetWeaponMagazineCapacity();
}
