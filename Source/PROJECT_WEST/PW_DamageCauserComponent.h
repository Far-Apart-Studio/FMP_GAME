// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DamageEvents.h"
#include "PW_DamageCauserComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_DamageCauserComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPW_DamageCauserComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void Damage(AActor* damageableActor, float damageAmount);
	void LocalDamage(AActor* damageableActor, float damageAmount);
	UFUNCTION(Server, Reliable) void ServerDamage(AActor* damageableActor, float damageAmount);
	
};
