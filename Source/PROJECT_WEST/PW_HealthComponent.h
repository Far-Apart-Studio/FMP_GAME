// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PW_HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReachedHealthThreshold);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDeath, AActor*, DamageCauser,AController*, DamageCauserController);

USTRUCT()
struct FHealthMilestone
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Health Handler")
	float HealthThreshold = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Health Handler")
	FOnReachedHealthThreshold OnReachHealthThreshold;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_HealthComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health Handler", meta = (AllowPrivateAccess = "true"))
	float _maxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health Handler", meta = (AllowPrivateAccess = "true"))
	float _minimumHealth = 0.0f;

	UPROPERTY(ReplicatedUsing = OnRep_OnHealthChange, EditAnywhere, BlueprintReadOnly, Category = "Health Handler", meta = (AllowPrivateAccess = "true"))
	float _currentHealth = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Health Handler")
	float _defaultHealth = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Health Handler")
	float _healthRecoveryRate = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Health Handler")
	float _healthRecoveryAmount = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Health Handler")
	float _healthRecoveryDelay = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Health Handler")
	bool _canRecoverHealth = false;

	UPROPERTY(Replicated,VisibleAnywhere, Category = "Health Handler")
	bool _isAlive = true;

	UPROPERTY(EditAnywhere, Category = "Health Handler")
	TArray<FHealthMilestone> _healthMilestones;
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Health Handler")
	FOnHealthChanged OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Health Handler")
	FOnDeath OnDeath;

public:	
	UPW_HealthComponent();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_OnHealthChange(float lastHealth);

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void TakeDamage(AActor* DamageActor, float DamageAmount, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	FORCEINLINE bool IsAlive() const { return _isAlive; }
};
