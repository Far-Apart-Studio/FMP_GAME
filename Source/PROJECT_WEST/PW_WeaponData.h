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

	UPROPERTY(EditAnywhere, Category = "Aimed Weapon Data")
	float _aWeaponFallOffRange;

	UPROPERTY(EditAnywhere, Category = "Aimed Weapon Data")
	int _aProjectileCount;

	UPROPERTY(EditAnywhere, Category = "Aimed Weapon Data")
	float _aProjectileDelay;

public:
	FORCEINLINE float GetWeaponMagazineCapacity() const { return _weaponMagazineCapacity; }
	FORCEINLINE float GetWeaponReserveAmmunition() const { return _weaponReserveAmmunition; }
	FORCEINLINE float GetWeaponReloadTime() const { return _weaponReloadTime; }

	FORCEINLINE EFireType GetHipWeaponFireType() const { return _hWeaponFireType; }
	FORCEINLINE float GetHipWeaponDamage() const { return _hWeaponDamage; }
	FORCEINLINE float GetHipWeaponFireRate() const { return _hWeaponFireRate; }
	FORCEINLINE FVector2D GetHipWeaponRecoil() const { return _hWeaponRecoil; }
	FORCEINLINE float GetHipWeaponAccuracy() const { return _hWeaponAccuracy; }
	FORCEINLINE float GetHipWeaponMaximumDistance() const { return _hWeaponMaximumDistance; }
	FORCEINLINE int GetHipProjectileCount() const { return _hProjectileCount; }
	FORCEINLINE float GetHipProjectileDelay() const { return _hProjectileDelay; }
	FORCEINLINE UCurveFloat* GetHipWeaponFallOffCurve() const { return _hWeaponFallOffCurve; }

	FORCEINLINE EFireType GetAimedWeaponFireType() const { return _aWeaponFireType; }
	FORCEINLINE float GetAimedWeaponDamage() const { return _aWeaponDamage; }
	FORCEINLINE float GetAimedWeaponFireRate() const { return _aWeaponFireRate; }
	FORCEINLINE FVector2D GetAimedWeaponRecoil() const { return _aWeaponRecoil; }
	FORCEINLINE float GetAimedWeaponAccuracy() const { return _aWeaponAccuracy; }
	FORCEINLINE float GetAimedWeaponFallOffRange() const { return _aWeaponFallOffRange; }
	FORCEINLINE int GetAimedProjectileCount() const { return _aProjectileCount; }
	FORCEINLINE float GetAimedProjectileDelay() const { return _aProjectileDelay; }
};
