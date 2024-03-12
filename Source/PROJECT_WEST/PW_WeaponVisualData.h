// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Sound/SoundCue.h"
#include "NiagaraSystem.h"
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
	UFXSystemAsset* _muzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Visual Data", meta = (AllowPrivateAccess = "true"))
	UFXSystemAsset* _projectileTracer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Sound Data", meta = (AllowPrivateAccess = "true"))
	USoundCue* _fireSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Sound Data", meta = (AllowPrivateAccess = "true"))
	USoundCue* _reloadSound;

public:
	UFUNCTION(BlueprintCallable) UNiagaraSystem* GetMuzzleFlashNiagara() { return Cast<UNiagaraSystem>(_muzzleFlash); }
	UFUNCTION(BlueprintCallable) UParticleSystem* GetMuzzleFlashParticle() { return Cast<UParticleSystem>(_muzzleFlash); }
	UFUNCTION(BlueprintCallable) UFXSystemAsset* GetMuzzleFlash() { return _muzzleFlash; }

	UFUNCTION(BlueprintCallable) UParticleSystem* GetProjectileTracerParticle() { return Cast<UParticleSystem>(_projectileTracer); }
	UFUNCTION(BlueprintCallable) UNiagaraSystem* GetProjectileTracerNiagara() { return Cast<UNiagaraSystem>(_projectileTracer); }
	UFUNCTION(BlueprintCallable) UFXSystemAsset* GetProjectileTracer() { return _projectileTracer; }
	
	FORCEINLINE USoundCue* GetFireSound() const { return _fireSound; }
	FORCEINLINE USoundCue* GetReloadSound() const { return _reloadSound; }
};
