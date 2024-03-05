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
	Aimed
};


UCLASS()
class PROJECT_WEST_API UPW_WeaponData : public UDataAsset
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "General Weapon Data")
	float _weaponMagazineCapacity;

	UPROPERTY(EditAnywhere, Category = "General Weapon Data")
	float _weaponReserveAmmunition;

	UPROPERTY(EditAnywhere, Category = "General Weapon Data")
	float _weaponReloadTime;

	//------------------ Hip Fire Weapon Data ------------------//
	
	UPROPERTY(EditAnywhere, Category = "Hip Weapon Data")
	EFireType _hWeaponFireType;
	
	UPROPERTY(EditAnywhere, Category = "Hip Weapon Data")
	float _hWeaponDamage;

	UPROPERTY(EditAnywhere, Category = "Hip Weapon Data")
	float _hWeaponFireRate;

	UPROPERTY(EditAnywhere, Category = "Hip Weapon Data")
	FVector2D _hWeaponRecoil;

	UPROPERTY(EditAnywhere, Category = "Hip Weapon Data")
	float _hWeaponAccuracy;

	UPROPERTY(EditAnywhere, Category = "Hip Weapon Data")
	float _hWeaponMaximumDistance;

	UPROPERTY(EditAnywhere, Category = "Hip Weapon Data")
	UCurveFloat* _hWeaponFallOffCurve;
	
	UPROPERTY(EditAnywhere, Category = "Hip Weapon Data")
	int _hProjectileCount;

	UPROPERTY(EditAnywhere, Category = "Hip Weapon Data")
	float _hProjectileDelay;

	//------------------ Aimed Weapon Data ------------------//

	UPROPERTY(EditAnywhere, Category = "Aim Weapon Data")
	EFireType _aWeaponFireType;
	
	UPROPERTY(EditAnywhere, Category = "Aim Weapon Data")
	float _aWeaponDamage;

	UPROPERTY(EditAnywhere, Category = "Aim Weapon Data")
	float _aWeaponFireRate;

	UPROPERTY(EditAnywhere, Category = "Aim Weapon Data")
	FVector2D _aWeaponRecoil;

	UPROPERTY(EditAnywhere, Category = "Aim Weapon Data")
	float _aWeaponAccuracy;

	UPROPERTY(EditAnywhere, Category = "Aim Weapon Data")
	float _aWeaponMaximumDistance;

	UPROPERTY(EditAnywhere, Category = "Aim Weapon Data")
	UCurveFloat* _aWeaponFallOffCurve;
	
	UPROPERTY(EditAnywhere, Category = "Aim Weapon Data")
	int _aProjectileCount;

	UPROPERTY(EditAnywhere, Category = "Hip Weapon Data")
	float _aProjectileDelay;

public:
	FORCEINLINE float GetWeaponMagazineCapacity() const { return _weaponMagazineCapacity; }
	FORCEINLINE float GetWeaponReserveAmmunition() const { return _weaponReserveAmmunition; }
	FORCEINLINE float GetWeaponReloadTime() const { return _weaponReloadTime; }

	FORCEINLINE EFireType GetWeaponFireType(EFireMode fireMode) const { return fireMode == EFireMode::Hip ? _hWeaponFireType : _aWeaponFireType; }
	FORCEINLINE float GetWeaponDamage(EFireMode fireMode) const { return fireMode == EFireMode::Hip ? _hWeaponDamage : _aWeaponDamage; }
	FORCEINLINE float GetWeaponFireRate(EFireMode fireMode) const { return fireMode == EFireMode::Hip ? _hWeaponFireRate : _aWeaponFireRate; }
	FORCEINLINE FVector2D GetWeaponRecoil(EFireMode fireMode) const { return fireMode == EFireMode::Hip ? _hWeaponRecoil : _aWeaponRecoil; }
	FORCEINLINE float GetWeaponAccuracy(EFireMode fireMode) const { return fireMode == EFireMode::Hip ? _hWeaponAccuracy : _aWeaponAccuracy; }
	FORCEINLINE float GetWeaponMaximumDistance(EFireMode fireMode) const { return fireMode == EFireMode::Hip ? _hWeaponMaximumDistance : _aWeaponMaximumDistance; }
	FORCEINLINE UCurveFloat* GetWeaponFallOffCurve(EFireMode fireMode) const { return fireMode == EFireMode::Hip ? _hWeaponFallOffCurve : _aWeaponFallOffCurve; }
	FORCEINLINE int GetProjectileCount(EFireMode fireMode) const { return fireMode == EFireMode::Hip ? _hProjectileCount : _aProjectileCount; }
	FORCEINLINE float GetProjectileDelay(EFireMode fireMode) const { return fireMode == EFireMode::Hip ? _hProjectileDelay : _aProjectileDelay; }
};
