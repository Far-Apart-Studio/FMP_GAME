// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PW_Weapon.generated.h"

UCLASS()
class PROJECT_WEST_API APW_Weapon : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	class UPW_WeaponData* _weaponData;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	int _currentAmmo;
	
public:	
	APW_Weapon();

	FORCEINLINE UPW_WeaponData* GetWeaponData() const { return _weaponData; }
	FORCEINLINE void SetWeaponData(UPW_WeaponData* weaponData) { _weaponData = weaponData; }
	
	FORCEINLINE int GetCurrentAmmo() const { return _currentAmmo; }
	FORCEINLINE void SetCurrentAmmo(int currentAmmo) { _currentAmmo = currentAmmo; }
	FORCEINLINE void SubtractCurrentAmmo(int amount) { _currentAmmo -= amount; }

	FORCEINLINE bool IsAmmoEmpty() const { return _currentAmmo <= 0; }

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
