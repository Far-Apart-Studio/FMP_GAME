// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PW_WeaponVisualData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API UPW_WeaponVisualData : public UDataAsset
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Visual Data")
	USkeletalMesh* _weaponMesh;

	UPROPERTY(EditAnywhere, Category = "Weapon Visual Data")
	UParticleSystem* _muzzleFlash;
	
	UPROPERTY(EditAnywhere, Category = "Weapon Visual Data")
	FRotator _weaponMeshRotation;

	UPROPERTY(EditAnywhere, Category = "Weapon Visual Data")
	FVector _weaponMeshOffset;

	UPROPERTY(EditAnywhere, Category = "Weapon Visual Data")
	FVector _weaponMeshScale;

	UPROPERTY(EditAnywhere, Category = "Weapon Visual Data")
	FVector _muzzleFlashOffset;

	UPROPERTY(EditAnywhere, Category = "Weapon Visual Data")
	FVector _muzzleFlashScale;

	UPROPERTY(EditAnywhere, Category = "Weapon Visual Data")
	FRotator _muzzleFlashRotation;

	
public:
	FORCEINLINE USkeletalMesh* GetWeaponMesh() const { return _weaponMesh; }
	FORCEINLINE FRotator GetWeaponMeshRotation() const { return _weaponMeshRotation; }
	FORCEINLINE FVector GetWeaponMeshOffset() const { return _weaponMeshOffset; }
	FORCEINLINE FVector GetWeaponMeshScale() const { return _weaponMeshScale; }
	FORCEINLINE UParticleSystem* GetMuzzleFlash() const { return _muzzleFlash; }
	FORCEINLINE FVector GetMuzzleFlashOffset() const { return _muzzleFlashOffset; }
	FORCEINLINE FVector GetMuzzleFlashScale() const { return _muzzleFlashScale; }
	FORCEINLINE FRotator GetMuzzleFlashRotation() const { return _muzzleFlashRotation; }
};
