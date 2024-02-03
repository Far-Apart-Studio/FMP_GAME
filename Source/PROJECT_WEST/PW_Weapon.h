// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW_WeaponData.h"
#include "PW_WeaponVisualData.h"
#include "PROJECT_WEST/Items/PW_Item.h"
#include "GameFramework/Actor.h"
#include "PW_Weapon.generated.h"

UCLASS()
class PROJECT_WEST_API APW_Weapon : public APW_Item
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UPW_WeaponData* _weaponData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UPW_WeaponVisualData* _weaponVisualData;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* _currentWeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* _currentMuzzleEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	bool _isReloading = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	bool _canFire = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	int _currentAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	int _currentReserveAmmo;
	
public:	
	APW_Weapon();
	void OnPicked() override;
	FORCEINLINE UPW_WeaponData* GetWeaponData() const { return _weaponData; }
	FORCEINLINE void SetWeaponData(UPW_WeaponData* weaponData) { _weaponData = weaponData; }
	FORCEINLINE UPW_WeaponVisualData* GetWeaponVisualData() const { return _weaponVisualData; }
	FORCEINLINE void SetWeaponVisualData(UPW_WeaponVisualData* weaponVisualData) { _weaponVisualData = weaponVisualData; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return _currentWeaponMesh; }
	FORCEINLINE void SetWeaponMesh(USkeletalMeshComponent* weaponMesh) { _currentWeaponMesh = weaponMesh; }
	FORCEINLINE UParticleSystemComponent* GetMuzzleEffect() const { return _currentMuzzleEffect; }
	FORCEINLINE void SetMuzzleEffect(UParticleSystemComponent* muzzleEffect) { _currentMuzzleEffect = muzzleEffect; }
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
	FORCEINLINE bool CanFire() const { return _canFire; }
	FORCEINLINE void SetCanFire(bool canFire) { _canFire = canFire; }

	void TransferReserveAmmo();
	void InitialiseWeapon(UPW_WeaponData* weaponData, UPW_WeaponVisualData* weaponVisualData);
	void InitialiseWeaponVisualData(UPW_WeaponVisualData* weaponVisualData);
	void InitialiseWeaponData(UPW_WeaponData* weaponData);

protected:
	virtual void BeginPlay() override;
	void OnVisibilityChange(bool bIsVisible) override; 
	
public:	
	virtual void Tick(float DeltaTime) override;

};
