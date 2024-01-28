// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW_WeaponData.h"
#include "GameFramework/Actor.h"
#include "PW_Weapon.generated.h"

UCLASS()
class PROJECT_WEST_API APW_Weapon : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Weapon")
	class UPW_WeaponData* _weaponData;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	USkeletalMeshComponent* _weaponMesh;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	int _currentAmmo;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	int _currentReserveAmmo;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	bool _isReloading = false;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	bool _canFire = true;
	
public:	
	APW_Weapon();

	FORCEINLINE UPW_WeaponData* GetWeaponData() const { return _weaponData; }
	FORCEINLINE void SetWeaponData(UPW_WeaponData* weaponData) { _weaponData = weaponData; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return _weaponMesh; }
	FORCEINLINE void SetWeaponMesh(USkeletalMeshComponent* weaponMesh) { _weaponMesh = weaponMesh; }
	FORCEINLINE int GetCurrentAmmo() const { return _currentAmmo; }
	FORCEINLINE void SetCurrentAmmo(int currentAmmo) { _currentAmmo = currentAmmo; }
	FORCEINLINE void SubtractCurrentAmmo(int amount) { _currentAmmo -= amount; }
	FORCEINLINE void ResetCurrentAmmo() { _currentAmmo = _weaponData->GetWeaponMagazineCapacity(); }
	FORCEINLINE int GetCurrentReserveAmmo() const { return _currentReserveAmmo; }
	FORCEINLINE void SetCurrentReserveAmmo(int currentReserveAmmo) { _currentReserveAmmo = currentReserveAmmo; }
	FORCEINLINE void SubtractCurrentReserveAmmo(int amount) { _currentReserveAmmo -= amount; }
	FORCEINLINE void ResetCurrentReserveAmmo() { _currentReserveAmmo = _weaponData->GetWeaponReserveAmmunition(); }
	FORCEINLINE bool IsAmmoEmpty() const { return _currentAmmo <= 0; }
	FORCEINLINE bool IsAmmoFull() const { return _currentAmmo == _weaponData->GetWeaponMagazineCapacity(); }
	FORCEINLINE bool IsReserveAmmoEmpty() const { return _currentReserveAmmo <= 0; }
	FORCEINLINE bool IsReserveAmmoFull() const { return _currentReserveAmmo == _weaponData->GetWeaponReserveAmmunition(); }
	FORCEINLINE bool IsReloading() const { return _isReloading; }
	FORCEINLINE void SetReloading(bool isReloading) { _isReloading = isReloading; }

	void TransferReserveAmmo();
	void InitialiseWeapon(UPW_WeaponData* weaponData);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
