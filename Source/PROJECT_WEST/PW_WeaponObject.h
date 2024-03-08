// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW_ItemObject.h"
#include "PW_WeaponData.h"
#include "PW_WeaponObject.generated.h"

class UPW_WeaponVisualData;
class UPW_WeaponData;
class APW_Weapon;
class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWeaponDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FWeaponSuccessfulFireDelegate, FHitResult, HitResult, FVector, StartPoint, FVector, EndPoint);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponHitDelegate, FHitResult, HitResult);

USTRUCT(BlueprintType)
struct FWeaponObjectRuntimeData
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float LastFiredTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool IsFiring;
};

UCLASS()
class PROJECT_WEST_API APW_WeaponObject : public APW_ItemObject
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	EFireMode _weaponFireMode = EFireMode::Hip;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UPW_WeaponData* _weaponData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UPW_WeaponVisualData* _weaponVisualData;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	USceneComponent* _muzzleLocation;
	
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* _weaponMesh;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FWeaponObjectRuntimeData _weaponRuntimeData;
	
	FTimerHandle _reloadTimerHandle;
	FTimerHandle _fireTimerHandle;

public:
	UPROPERTY(BlueprintAssignable, Category = "Weapon Delegates")
	FWeaponDelegate OnWeaponFireDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Weapon Delegates")
	FWeaponDelegate OnWeaponStopFire;

	UPROPERTY(BlueprintAssignable, Category = "Weapon Delegates")
	FWeaponDelegate OnWeaponBeginReload;

	UPROPERTY(BlueprintAssignable, Category = "Weapon Delegates")
	FWeaponDelegate OnWeaponCompleteReload;

	UPROPERTY(BlueprintAssignable, Category = "Weapon Delegates")
	FWeaponDelegate OnWeaponCancelReload;

	UPROPERTY(BlueprintAssignable, Category = "Weapon Delegates")
	FWeaponDelegate OnWeaponEquip;

	UPROPERTY(BlueprintAssignable, Category = "Weapon Delegates")
	FWeaponDelegate OnWeaponUnEquip;

	UPROPERTY(BlueprintAssignable, Category = "Weapon Delegates")
	FWeaponDelegate OnHitDamageable;

	UPROPERTY(BlueprintAssignable, Category = "Weapon Delegates")
	FWeaponSuccessfulFireDelegate OnWeaponSuccessfulFire;

	UPROPERTY(BlueprintAssignable, Category = "Weapon Delegates")
	FWeaponHitDelegate OnWeaponHit;

public:
	APW_WeaponObject();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void LocalApplyActionBindings(APW_Character* characterOwner) override;
	virtual void LocalRemoveActionBindings(APW_Character* characterOwner) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void CastBulletRays();
	void CastBulletRay(UCameraComponent* cameraComponent);
	void SimulateBulletSpread(FVector& rayDirection);
	bool CastRay(const FVector& rayStart, const FVector& rayDestination, const FCollisionQueryParams& collisionQueryParams, FHitResult& hitResult) const;
	void ApplyDamage(const FHitResult& hitResult);
	void LocalApplyDamage(const FHitResult& hitResult);
	float CalculateDamage(const FHitResult& hitResult);
	bool CanFire();
	void LocalReloadWeapon();
	void OnReloadWeaponComplete();
	void CoreFireSequence();
	void QueueAutomaticFire();
	void TransferReserveAmmo();
	void QueueWeaponRecoil();

	UFUNCTION() void CompleteWeaponRecoil();
	UFUNCTION() void BeginFireSequence();
	UFUNCTION() void CompleteFireSequence();
	UFUNCTION() void ReloadWeapon();
	
	UFUNCTION(Server, Reliable) void ServerReloadWeapon();
	UFUNCTION(Server, Reliable) void ServerApplyDamage(const FHitResult& hitResult);

	FORCEINLINE bool IsAmmoEmpty() const { return _weaponRuntimeData.CurrentAmmo <= 0; }
	FORCEINLINE bool IsAmmoFull() const { return _weaponRuntimeData.CurrentAmmo == _weaponData->GetWeaponMagazineCapacity(); }
	FORCEINLINE bool IsReserveAmmoEmpty() const { return _weaponRuntimeData.CurrentReserveAmmo <= 0; }
	FORCEINLINE bool IsReserveAmmoFull() const { return _weaponRuntimeData.CurrentReserveAmmo == _weaponData->GetWeaponReserveAmmunition(); }
	FORCEINLINE void SetFireMode(EFireMode fireMode) { _weaponFireMode = fireMode; }
};
