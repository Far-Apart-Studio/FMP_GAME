// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PW_WeaponData.generated.h"


UENUM()
enum class EFireType : uint8
{
	Automatic,
	Manual
};

UENUM()
enum class EFireMode : uint8
{
	Hip,
	Aim
};

UCLASS()
class PROJECT_WEST_API UPW_WeaponData : public UDataAsset
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "General Weapon Data")
	float _weaponMagazineCapacity = 10;

	UPROPERTY(EditAnywhere, Category = "General Weapon Data")
	float _weaponReserveAmmunition = 90;

	UPROPERTY(EditAnywhere, Category = "General Weapon Data")
	float _weaponReloadTime = 0.5f;

	//------------------ Hip Fire Weapon Data ------------------//
	
	UPROPERTY(EditAnywhere, Category = "Hip Weapon Data")
	EFireType _hWeaponFireType = EFireType::Automatic;
	
	UPROPERTY(EditAnywhere, Category = "Hip Weapon Data")
	float _hWeaponDamage = 10;

	UPROPERTY(EditAnywhere, Category = "Hip Weapon Data")
	float _hWeaponFireRate = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Hip Weapon Data")
	FVector2D _hMaximumWeaponRecoil = FVector2D(10, -10);

	UPROPERTY(EditAnywhere, Category = "Hip Weapon Data")
	FVector2D _hMinimumWeaponRecoil = FVector2D(-10, -7);

	UPROPERTY(EditAnywhere, Category = "Hip Weapon Data")
	float _hWeaponRecoilSpeed = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Hip Weapon Data")
	float _hWeaponAccuracy = 0.025f;

	UPROPERTY(EditAnywhere, Category = "Hip Weapon Data")
	float _hWeaponMaximumDistance = 10000;

	UPROPERTY(EditAnywhere, Category = "Hip Weapon Data")
	UCurveFloat* _hWeaponFallOffCurve;
	
	UPROPERTY(EditAnywhere, Category = "Hip Weapon Data")
	int _hProjectileCount = 1;

	UPROPERTY(EditAnywhere, Category = "Hip Weapon Data")
	float _hProjectileDelay = 0;

	UPROPERTY(EditAnywhere, Category = "Hip Weapon Data")
	float _hAmmoConsumption = 1;

	//------------------ Aimed Weapon Data ------------------//

	UPROPERTY(EditAnywhere, Category = "Aim Weapon Data")
	EFireType _aWeaponFireType;
	
	UPROPERTY(EditAnywhere, Category = "Aim Weapon Data")
	float _aWeaponDamage;

	UPROPERTY(EditAnywhere, Category = "Aim Weapon Data")
	float _aWeaponFireRate;

	UPROPERTY(EditAnywhere, Category = "Aim Weapon Data")
	FVector2D _aMaximumWeaponRecoil;

	UPROPERTY(EditAnywhere, Category = "Aim Weapon Data")
	FVector2D _aMinimumWeaponRecoil;

	UPROPERTY(EditAnywhere, Category = "Aim Weapon Data")
	float _aWeaponRecoilSpeed;

	UPROPERTY(EditAnywhere, Category = "Aim Weapon Data")
	float _aWeaponAccuracy;

	UPROPERTY(EditAnywhere, Category = "Aim Weapon Data")
	float _aWeaponMaximumDistance;

	UPROPERTY(EditAnywhere, Category = "Aim Weapon Data")
	UCurveFloat* _aWeaponFallOffCurve;
	
	UPROPERTY(EditAnywhere, Category = "Aim Weapon Data")
	int _aProjectileCount;

	UPROPERTY(EditAnywhere, Category = "Aim Weapon Data")
	float _aProjectileDelay;

	UPROPERTY(EditAnywhere, Category = "Aim Weapon Data")
	float _aAmmoConsumption;

	//------------------ Player Modifiers ------------------//

	UPROPERTY(EditAnywhere, Category = "Player Modifiers")
	float _aMovementSpeedModifier;

	UPROPERTY(EditAnywhere, Category = "Player Modifiers")
	float _aFieldOfViewModifier;

public:
	FORCEINLINE float GetWeaponMagazineCapacity() const { return _weaponMagazineCapacity; }
	FORCEINLINE float GetWeaponReserveAmmunition() const { return _weaponReserveAmmunition; }
	FORCEINLINE float GetWeaponReloadTime() const { return _weaponReloadTime; }
	FORCEINLINE float GetMovementSpeedModifier() const { return _aMovementSpeedModifier; }
	FORCEINLINE float GetFieldOfViewModifier() const { return _aFieldOfViewModifier; }

	FORCEINLINE EFireType GetWeaponFireType(EFireMode fireMode) const { return fireMode == EFireMode::Hip ? _hWeaponFireType : _aWeaponFireType; }
	FORCEINLINE float GetWeaponDamage(EFireMode fireMode) const { return fireMode == EFireMode::Hip ? _hWeaponDamage : _aWeaponDamage; }
	FORCEINLINE float GetWeaponFireRate(EFireMode fireMode) const { return fireMode == EFireMode::Hip ? _hWeaponFireRate : _aWeaponFireRate; }
	FORCEINLINE FVector2D GetMinimumWeaponRecoil(EFireMode fireMode) const{ return fireMode == EFireMode::Hip ? _hMinimumWeaponRecoil : _aMinimumWeaponRecoil; }
	FORCEINLINE FVector2D GetMaximumWeaponRecoil(EFireMode fireMode) const { return fireMode == EFireMode::Hip ? _hMaximumWeaponRecoil : _aMaximumWeaponRecoil; }
	FORCEINLINE float GetWeaponAccuracy(EFireMode fireMode) const { return fireMode == EFireMode::Hip ? _hWeaponAccuracy : _aWeaponAccuracy; }
	FORCEINLINE float GetWeaponMaximumDistance(EFireMode fireMode) const { return fireMode == EFireMode::Hip ? _hWeaponMaximumDistance : _aWeaponMaximumDistance; }
	FORCEINLINE UCurveFloat* GetWeaponFallOffCurve(EFireMode fireMode) const { return fireMode == EFireMode::Hip ? _hWeaponFallOffCurve : _aWeaponFallOffCurve; }
	FORCEINLINE int GetProjectileCount(EFireMode fireMode) const { return fireMode == EFireMode::Hip ? _hProjectileCount : _aProjectileCount; }
	FORCEINLINE float GetProjectileDelay(EFireMode fireMode) const { return fireMode == EFireMode::Hip ? _hProjectileDelay : _aProjectileDelay; }
	FORCEINLINE float GetWeaponRecoilSpeed(EFireMode fireMode) const { return fireMode == EFireMode::Hip ? _hWeaponRecoilSpeed : _aWeaponRecoilSpeed; }
	FORCEINLINE float GetAmmoConsumption(EFireMode fireMode) const { return fireMode == EFireMode::Hip ? _hAmmoConsumption : _aAmmoConsumption; }

	const FString MAXIMUM_RECOIL_APPLICATION_COUNT_TOOLTIP
		= "The maximum number of times recoil can be applied per shot. Usually recoil is applied every bullet.";
};
