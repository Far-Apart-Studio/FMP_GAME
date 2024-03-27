// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW_DamageCauserComponent.h"
#include "PW_SphereDamageCauser.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_SphereDamageCauser : public UPW_DamageCauserComponent
{
	GENERATED_BODY()

private:
	UPROPERTY() TArray<AActor*> _actorsDamaged;
	UPROPERTY(EditAnywhere, Category = "Damage Causer") float _damageRadius = 100.0f;
	
public:
	UFUNCTION(BlueprintCallable) void GetActorsWithinBounds();
	UFUNCTION(BlueprintCallable) void DamageActorsWithinBounds(float damageAmount);
};
