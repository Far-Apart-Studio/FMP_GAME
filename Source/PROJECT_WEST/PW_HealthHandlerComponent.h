// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PW_HealthHandlerComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_HealthHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Health Handler")
	float _maxHealth = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Health Handler")
	float _minimumHealth = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Health Handler")
	float _currentHealth = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Health Handler")
	float _defaultHealth = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Health Handler")
	float _healthRecoveryRate = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Health Handler")
	float _healthRecoveryAmount = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Health Handler")
	float _healthRecoveryDelay = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Health Handler")
	bool _canRecoverHealth = false;

	UPROPERTY(EditAnywhere, Category = "Health Handler")
	TArray<class UPW_HealthThreshold*> _healthThresholds;

public:	
	UPW_HealthHandlerComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void TakeDamage(AActor* DamageActor, float DamageAmount, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
};
