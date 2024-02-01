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
	
public:
	FORCEINLINE USkeletalMesh* GetWeaponMesh() const { return _weaponMesh; }
	
};
