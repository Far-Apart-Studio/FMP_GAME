// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PW_HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHealthDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnDamageReceivedDelegate, AActor*, OwnerActor, AActor*, DamageCauser, AController*, DamageCauserController, float, DamageAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDeathDelegate, AActor*, OwnerActor, AActor*, DamageCauser, AController*, DamageCauserController);

USTRUCT()
struct FHealthMilestone
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Health Handler")
	float HealthThreshold = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Health Handler")
	FHealthDelegate OnReachHealthThreshold;
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

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Health Handler", meta = (AllowPrivateAccess = "true"))
	float _currentHealth = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Health Handler")
	float _defaultHealth = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Health Handler")
	float _recoveryMaximumHealth = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Health Handler")
	float _healthRecoveryRate = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Health Handler")
	float _healthRecoveryAmount = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Health Handler")
	float _combatRecoveryDelay = 0.0f;

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Health Handler")
	bool _isAlive = true;

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Health Handler")
	bool _isInvulnerable = false;

	float _lastRecoveredHealth = 0.0f;
	float _lastTakenDamage = 0.0f;
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Health Handler")
	FHealthDelegate OnHealthChangedGlobal;

	UPROPERTY(BlueprintAssignable, Category = "Health Handler")
	FHealthDelegate OnHealthChangedLocal;
	
	UPROPERTY(BlueprintAssignable, Category = "Health Handler")
	FOnDamageReceivedDelegate OnDamageReceivedGlobal;

	UPROPERTY(BlueprintAssignable, Category = "Health Handler")
	FOnDamageReceivedDelegate OnDamageReceivedLocal;

	UPROPERTY(BlueprintAssignable, Category = "Health Handler")
	FOnDamageReceivedDelegate OnHealingReceivedGlobal;

	UPROPERTY(BlueprintAssignable, Category = "Health Handler")
	FOnDamageReceivedDelegate OnHealingReceivedLocal;
	
	UPROPERTY(BlueprintAssignable, Category = "Health Handler")
	FOnDeathDelegate OnDeathGlobal;

	UPROPERTY(BlueprintAssignable, Category = "Health Handler")
	FOnDeathDelegate OnDeathLocal;

	UPROPERTY(BlueprintAssignable, Category = "Health Handler")
	FHealthDelegate OnInvulnerabilityGlobal;

public:
	
	UPW_HealthComponent();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void RecoverHealth(float recoverValue);
	void LocalRecoverHealth(float recoverValue);
	void LocalTakeDamage(AActor* OwnerActor, float DamageAmount, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
	void SetIsInvulnerable(bool isInvulnerable);
	void LocalSetIsInvulnerable(bool isInvulnerable);
	void RegenerateHealth();
	bool CanReceiveDamage(float damageAmount) const;
	bool CanRecoverHealth();
	
	UFUNCTION() void TakeDamage(AActor* OwnerActor, float DamageAmount, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
	UFUNCTION(Server, Reliable) void ServerRecoverHealth(float recoverValue);
	UFUNCTION(Server, Reliable) void ServerTakeDamage(AActor* OwnerActor, float DamageAmount, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
	UFUNCTION(Server, Reliable) void ServerSetIsInvulnerable(bool isInvulnerable);
	UFUNCTION(NetMulticast, Reliable) void MulticastHealthModified(AActor* OwnerActor, AActor* DamageCauser, AController* DamageCauserController, float ModificationAmount);

	FORCEINLINE bool IsAlive() const { return _isAlive; }
	FORCEINLINE bool IsInvulnerable() const { return _isInvulnerable; }
};
