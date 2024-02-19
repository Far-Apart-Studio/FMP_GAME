// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW_ItemObject.h"
#include "PW_WeaponObject.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API APW_WeaponObject : public APW_ItemObject
{
	GENERATED_BODY()

private:

public:
	virtual void ApplyBindingActions(APW_Character* characterOwner) override;
	virtual void RemoveBindingActions(APW_Character* characterOwner) override;
	
	UFUNCTION() void Reload();
	UFUNCTION() void Fire();
	UFUNCTION() void Aim();
	
};
