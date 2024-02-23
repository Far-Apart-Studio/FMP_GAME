// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_Weapon.h"

#include "PW_Utilities.h"
#include "PW_WeaponData.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"

APW_Weapon::APW_Weapon()
{
	PrimaryActorTick.bCanEverTick = false;
	
	_currentWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	_currentWeaponMesh->SetupAttachment(RootComponent);

	_currentMuzzleEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MuzzleEffect"));
	_currentMuzzleEffect->SetupAttachment(_currentWeaponMesh);
}

void APW_Weapon::OnPicked()
{
	Super::OnPicked();
	InitialiseWeaponVisualData(_weaponVisualData);
}

void APW_Weapon::BeginPlay()
{
	Super::BeginPlay();

	_currentMuzzleEffect->SetIsReplicated(true);
	SetReplicateMovement(true);
	SetReplicates(true);
	InitialiseWeapon(_weaponData, _weaponVisualData);
}

void APW_Weapon::OnVisibilityChange(bool bIsVisible)
{
	_currentWeaponMesh->SetVisibility(bIsVisible);
}

void APW_Weapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APW_Weapon, _weaponRuntimeData);
}

void APW_Weapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APW_Weapon::TransferReserveAmmo()
{
	if (_weaponRuntimeData.CurrentReserveAmmo <= 0)
		return;

	const int ammoRequired = _weaponData->GetWeaponMagazineCapacity() - _weaponRuntimeData.CurrentAmmo;
	const int ammoToTransfer = FMath::Min(ammoRequired, _weaponRuntimeData.CurrentReserveAmmo);
	
	_weaponRuntimeData.CurrentAmmo += ammoToTransfer;
	_weaponRuntimeData.CurrentReserveAmmo -= ammoToTransfer;
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

	if(_currentMuzzleEffect)
		_currentMuzzleEffect->SetTemplate(_weaponVisualData->GetMuzzleFlash());
}

void APW_Weapon::InitialiseWeaponData(UPW_WeaponData* weaponData)
{
	_weaponData = weaponData;
	
	if (_weaponData == nullptr)
		{ PW_Utilities::Log("FAILED TO LOAD WEAPON DATA"); return; }
	
	_weaponRuntimeData.CurrentAmmo = _weaponData->GetWeaponMagazineCapacity();
	_weaponRuntimeData.CurrentReserveAmmo = _weaponData->GetWeaponReserveAmmunition();
}
