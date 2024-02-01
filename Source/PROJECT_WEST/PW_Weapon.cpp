// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_Weapon.h"

#include "PW_Utilities.h"
#include "PW_WeaponData.h"
#include "Particles/ParticleSystemComponent.h"

APW_Weapon::APW_Weapon()
{
	PrimaryActorTick.bCanEverTick = true;

	_currentWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	_currentWeaponMesh->SetupAttachment(RootComponent);

	_currentMuzzleEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MuzzleEffect"));
	_currentMuzzleEffect->SetupAttachment(_currentWeaponMesh);
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


void APW_Weapon::InitialiseWeapon(UPW_WeaponData* weaponData, UPW_WeaponVisualData* weaponVisualData)
{
	InitialiseWeaponData(weaponData);
	InitialiseWeaponVisualData(weaponVisualData);
}

void APW_Weapon::InitialiseWeaponVisualData(UPW_WeaponVisualData* weaponVisualData)
{
	_weaponVisualData = weaponVisualData;
	
	if (_weaponVisualData == nullptr)
		{ PW_Utilities::Log("FAILED TO LOAD WEAPON VISUAL DATA"); return; }
	
	_currentWeaponMesh->SetSkeletalMesh(_weaponVisualData->GetWeaponMesh());
	_currentWeaponMesh->SetRelativeRotation(_weaponVisualData->GetWeaponMeshRotation());
	_currentWeaponMesh->SetRelativeLocation(_weaponVisualData->GetWeaponMeshOffset());
	_currentWeaponMesh->SetRelativeScale3D(_weaponVisualData->GetWeaponMeshScale());
	_currentMuzzleEffect->SetTemplate(_weaponVisualData->GetMuzzleFlash());
}

void APW_Weapon::InitialiseWeaponData(UPW_WeaponData* weaponData)
{
	_weaponData = weaponData;
	
	if (_weaponData == nullptr)
		{ PW_Utilities::Log("FAILED TO LOAD WEAPON DATA"); return; }
	
	_currentAmmo = _weaponData->GetWeaponMagazineCapacity();
	_currentReserveAmmo = _weaponData->GetWeaponReserveAmmunition();
}
