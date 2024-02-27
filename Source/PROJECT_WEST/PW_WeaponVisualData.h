// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Sound/SoundCue.h"
#include "PW_WeaponVisualData.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API UPW_WeaponVisualData : public UDataAsset
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Visual Data", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* _muzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Sound Data", meta = (AllowPrivateAccess = "true"))
	USoundCue* _fireSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Sound Data", meta = (AllowPrivateAccess = "true"))
	USoundCue* _reloadSound;

public:
	FORCEINLINE UParticleSystem* GetMuzzleFlash() const { return _muzzleFlash; }
	FORCEINLINE USoundCue* GetFireSound() const { return _fireSound; }
	FORCEINLINE USoundCue* GetReloadSound() const { return _reloadSound; }
};
