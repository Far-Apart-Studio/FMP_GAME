// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW_WeaponData.h"
#include "PW_WeaponVisualData.h"
#include "PROJECT_WEST/Items/PW_Item.h"
#include "GameFramework/Actor.h"
#include "PW_Weapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWeaponActionDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FWeaponActionHitDelegate, FHitResult, HitResult, FVector, StartPoint, FVector, EndPoint);

USTRUCT(BlueprintType)
struct FWeaponRuntimeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool CanFire;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool IsReloading;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	int CurrentAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	int CurrentReserveAmmo;
};


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
	USceneComponent* _muzzleLocation;
	
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FWeaponRuntimeData _weaponRuntimeData;

public:
	UPROPERTY(BlueprintAssignable, Category = "Weapon Delegates")
	FWeaponActionDelegate OnWeaponFireDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Weapon Delegates")
	FWeaponActionDelegate OnWeaponStopFire;

	UPROPERTY(BlueprintAssignable, Category = "Weapon Delegates")
	FWeaponActionDelegate OnWeaponBeginReload;

	UPROPERTY(BlueprintAssignable, Category = "Weapon Delegates")
	FWeaponActionDelegate OnWeaponCompleteReload;

	UPROPERTY(BlueprintAssignable, Category = "Weapon Delegates")
	FWeaponActionDelegate OnWeaponCancelReload;

	UPROPERTY(BlueprintAssignable, Category = "Weapon Delegates")
	FWeaponActionDelegate OnWeaponEquip;

	UPROPERTY(BlueprintAssignable, Category = "Weapon Delegates")
	FWeaponActionDelegate OnWeaponUnEquip;

	UPROPERTY(BlueprintAssignable, Category = "Weapon Delegates")
	FWeaponActionDelegate OnHitDamageable;

	UPROPERTY(BlueprintAssignable, Category = "Weapon Delegates")
	FWeaponActionHitDelegate OnWeaponHit;
	
public:	
	APW_Weapon();
	
	void OnPicked() override;
	FORCEINLINE UPW_WeaponData* GetWeaponData() const { return _weaponData; }
	FORCEINLINE void SetWeaponData(UPW_WeaponData* weaponData) { _weaponData = weaponData; }
	FORCEINLINE UPW_WeaponVisualData* GetWeaponVisualData() const { return _weaponVisualData; }
	FORCEINLINE void SetWeaponVisualData(UPW_WeaponVisualData* weaponVisualData) { _weaponVisualData = weaponVisualData; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return _currentWeaponMesh; }
	FORCEINLINE void SetWeaponMesh(USkeletalMeshComponent* weaponMesh) { _currentWeaponMesh = weaponMesh; }
	FORCEINLINE int GetCurrentAmmo() const { return _weaponRuntimeData.CurrentAmmo; }
	FORCEINLINE void SetCurrentAmmo(int currentAmmo) { _weaponRuntimeData.CurrentAmmo = currentAmmo; }
	FORCEINLINE void SubtractCurrentAmmo(int amount) { _weaponRuntimeData.CurrentAmmo -= amount; }
	FORCEINLINE void ResetCurrentAmmo() { _weaponRuntimeData.CurrentAmmo = _weaponData->GetWeaponMagazineCapacity(); }
	FORCEINLINE int GetCurrentReserveAmmo() const { return _weaponRuntimeData.CurrentReserveAmmo; }
	FORCEINLINE void SetCurrentReserveAmmo(int currentReserveAmmo) { _weaponRuntimeData.CurrentReserveAmmo = currentReserveAmmo; }
	FORCEINLINE void SubtractCurrentReserveAmmo(int amount) { _weaponRuntimeData.CurrentReserveAmmo -= amount; }
	FORCEINLINE void ResetCurrentReserveAmmo() { _weaponRuntimeData.CurrentReserveAmmo = _weaponData->GetWeaponReserveAmmunition(); }
	FORCEINLINE bool IsAmmoEmpty() const { return _weaponRuntimeData.CurrentAmmo <= 0; }
	FORCEINLINE bool IsAmmoFull() const { return _weaponRuntimeData.CurrentAmmo == _weaponData->GetWeaponMagazineCapacity(); }
	FORCEINLINE bool IsReserveAmmoEmpty() const { return _weaponRuntimeData.CurrentReserveAmmo <= 0; }
	FORCEINLINE bool IsReserveAmmoFull() const { return _weaponRuntimeData.CurrentReserveAmmo == _weaponData->GetWeaponReserveAmmunition(); }
	FORCEINLINE bool IsReloading() const { return _weaponRuntimeData.IsReloading; }
	FORCEINLINE void SetReloading(bool isReloading) { _weaponRuntimeData.IsReloading = isReloading; }
	FORCEINLINE bool CanFire() const { return _weaponRuntimeData.CanFire; }
	FORCEINLINE void SetCanFire(bool canFire) { _weaponRuntimeData.CanFire = canFire; }

	void TransferReserveAmmo();
	void InitialiseWeapon(UPW_WeaponData* weaponData, UPW_WeaponVisualData* weaponVisualData);
	void InitialiseWeaponVisualData(UPW_WeaponVisualData* weaponVisualData);
	void InitialiseWeaponData(UPW_WeaponData* weaponData);

protected:
	virtual void BeginPlay() override;
	virtual void OnVisibilityChange(bool bIsVisible) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	
public:	
	virtual void Tick(float DeltaTime) override;
};
