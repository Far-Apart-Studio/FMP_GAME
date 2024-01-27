// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PW_WeaponData.generated.h"


UENUM()
enum class EFireType : uint8
{
	Automatic,
	SemiAutomatic,
	Burst
};


UCLASS()
class PROJECT_WEST_API UPW_WeaponData : public UDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "General Weapon Data")
	float _weaponMagazineCapacity;

	UPROPERTY(EditAnywhere, Category = "General Weapon Data")
	float _weaponReserveAmmunition;

	UPROPERTY(EditAnywhere, Category = "General Weapon Data")
	float _weaponReloadTime;

	//------------------ Hip Fire Weapon Data ------------------//
	
	UPROPERTY(EditAnywhere, Category = "Hip Fire Weapon Data")
	EFireType _hWeaponFireType;
	
	UPROPERTY(EditAnywhere, Category = "Hip Fire Weapon Data")
	float _hWeaponDamage;

	UPROPERTY(EditAnywhere, Category = "Hip Fire Weapon Data")
	float _hWeaponFireRate;

	UPROPERTY(EditAnywhere, Category = "Hip Fire Weapon Data")
	FVector2D _hWeaponRecoil;

	UPROPERTY(EditAnywhere, Category = "Hip Fire Weapon Data")
	float _hWeaponAccuracy;

	//------------------ Aimed Weapon Data ------------------//

	UPROPERTY(EditAnywhere, Category = "Aimed Weapon Data")
	EFireType _aWeaponFireType;

	UPROPERTY(EditAnywhere, Category = "Aimed Weapon Data")
	float _aWeaponDamage;

	UPROPERTY(EditAnywhere, Category = "Aimed Weapon Data")
	float _aWeaponFireRate;

	UPROPERTY(EditAnywhere, Category = "Aimed Weapon Data")
	FVector2D _aWeaponRecoil;

	UPROPERTY(EditAnywhere, Category = "Aimed Weapon Data")
	float _aWeaponAccuracy;
};
