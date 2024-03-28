// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PW_HealthComponent.generated.h"

class APW_Character;

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

USTRUCT()
struct FRegenerationHandle
{
	GENERATED_BODY()

	DECLARE_DELEGATE_OneParam(FRegenerationMethod, float);
	DECLARE_DELEGATE_RetVal(bool, FRegenerationCondition);
	DECLARE_DELEGATE(FOnReach);

	UPROPERTY(EditAnywhere) bool AllowRegeneration;
	UPROPERTY(EditAnywhere, NotReplicated) float RegenerationMaximum;
	UPROPERTY(EditAnywhere, NotReplicated) float RegenerationMinimum;
	UPROPERTY(EditAnywhere, NotReplicated) float RegenerationRate;
	UPROPERTY(EditAnywhere, NotReplicated) float RegenerationAmount;
	UPROPERTY(EditAnywhere, NotReplicated) float InitialDelay;
	UPROPERTY(EditAnywhere, NotReplicated) UCurveFloat* RegenerationCurve;
	UPROPERTY(EditAnywhere, NotReplicated) bool ShouldResetOnFail;
	UPROPERTY(VisibleAnywhere, NotReplicated) float RegenerationTimer;
	UPROPERTY(VisibleAnywhere, NotReplicated) float InitialDelayTimer;

	FRegenerationMethod RegenerationMethod;
	FRegenerationCondition RegenerationCondition;
	FOnReach OnReachMinimum;
	FOnReach OnReachMaximum;

	FRegenerationHandle()
	{
		ShouldResetOnFail = true;
		InitialDelay = 3.0f;
		RegenerationCurve = nullptr;
		AllowRegeneration = false;
		RegenerationMinimum = 0.0f;
		RegenerationMaximum = 100.0f;
		RegenerationRate = 1.0f;
		RegenerationAmount = 2.0f;
		RegenerationTimer = 0.0f;
		InitialDelayTimer = 0.0f;
	}

	void Increase(float& currentHealth, float deltaTime)
	{
		RegenerationTimer += deltaTime;
		if (!CanIncrease())
			return;

		InitialDelayTimer += deltaTime;
		if (InitialDelayTimer < InitialDelay)
			return;

		const float normalisedHealth = currentHealth / RegenerationMaximum;
		float regenerationAmount = RegenerationAmount;

		if (RegenerationCurve != nullptr)
		{
			const float curveValue = RegenerationCurve->GetFloatValue(normalisedHealth);
			regenerationAmount = RegenerationAmount * curveValue;
		}

		if (RegenerationMethod.IsBound())
		{
			RegenerationMethod.Execute(regenerationAmount);
			RegenerationTimer = 0.0f;
		}
		else
		{
			currentHealth = FMath::Clamp
			(currentHealth + RegenerationAmount, RegenerationMinimum, RegenerationMaximum);
			RegenerationTimer = 0.0f;
		}

		if (currentHealth == RegenerationMinimum && OnReachMinimum.IsBound())
			OnReachMinimum.Execute();
		else if (currentHealth == RegenerationMaximum && OnReachMaximum.IsBound())
			OnReachMaximum.Execute();
	}

	bool CanIncrease()
	{
		bool canRegenerate = AllowRegeneration && RegenerationTimer >= RegenerationRate;

		if (RegenerationCondition.IsBound())
		{
			const bool regenerationCondition = RegenerationCondition.Execute();
			canRegenerate = canRegenerate && regenerationCondition;

			if (!regenerationCondition && ShouldResetOnFail)
				InitialDelayTimer = 0.0f;
		}
		
		return canRegenerate;
	}

	void Reset()
	{
		RegenerationTimer = 0.0f;
		InitialDelayTimer = 0.0f;
	}
};

USTRUCT()
struct FFallDamageData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	bool AllowFallDamage = true;
	
	UPROPERTY(EditAnywhere)
	float MinimumVelocity = 700.0f;

	UPROPERTY(EditAnywhere)
	float MaximumVelocity = 1000.0f;

	UPROPERTY(EditAnywhere)
	float DamageMultiplier = 0.02f;

	UPROPERTY(EditAnywhere)
	UCurveFloat* FallDamageCurve;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class PROJECT_WEST_API UPW_HealthComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health Handler", meta = (AllowPrivateAccess = "true"))
	APW_Character* _characterOwner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health Handler", meta = (AllowPrivateAccess = "true"))
	AController* _controller;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Health Handler", meta = (AllowPrivateAccess = "true"))
	AController* _lastInstigatedBy;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Health Handler", meta = (AllowPrivateAccess = "true"))
	AActor* _lastDamageCauser;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Health Handler", meta = (AllowPrivateAccess = "true"))
	int32 _lastDamageAmount;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health Handler", meta = (AllowPrivateAccess = "true"))
	float _maxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health Handler", meta = (AllowPrivateAccess = "true"))
	float _minimumHealth = 0.0f;

	UPROPERTY(ReplicatedUsing = OnRep_HealthChanged, EditAnywhere, BlueprintReadOnly, Category = "Health Handler", meta = (AllowPrivateAccess = "true"))
	float _currentHealth = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Health Handler")
	float _defaultHealth = 100.0f;

	UPROPERTY(Replicated, EditAnywhere, Category = "Health Handler")
	bool _isAlive = true;

	UPROPERTY(Replicated, EditAnywhere, Category = "Health Handler")
	bool _isInvulnerable = false;

	UPROPERTY(Replicated, EditAnywhere, Category = "Health Handler")
	FRegenerationHandle _regenerationHandle;

	UPROPERTY(EditAnywhere, Category = "Health Handler")
	FFallDamageData _fallDamageData;
	
public:
	
	UPROPERTY(BlueprintAssignable, Category = "Health Handler")
	FHealthDelegate OnHealthChangedLocal;
	
	UPROPERTY(BlueprintAssignable, Category = "Health Handler")
	FHealthDelegate OnHealthChangedGlobal;

	UPROPERTY(BlueprintAssignable, Category = "Health Handler")
	FHealthDelegate OnHealthChangedServer;

	UPROPERTY(BlueprintAssignable, Category = "Health Handler")
	FOnDamageReceivedDelegate OnDamageReceivedLocal;
	
	UPROPERTY(BlueprintAssignable, Category = "Health Handler")
	FOnDamageReceivedDelegate OnDamageReceivedGlobal;

	UPROPERTY(BlueprintAssignable, Category = "Health Handler")
	FOnDamageReceivedDelegate OnDamageReceivedServer;

	UPROPERTY(BlueprintAssignable, Category = "Health Handler")
	FOnDamageReceivedDelegate OnHealingReceivedLocal;

	UPROPERTY(BlueprintAssignable, Category = "Health Handler")
	FOnDamageReceivedDelegate OnHealingReceivedGlobal;

	UPROPERTY(BlueprintAssignable, Category = "Health Handler")
	FOnDamageReceivedDelegate OnHealingReceivedServer;

	UPROPERTY(BlueprintAssignable, Category = "Health Handler")
	FOnDeathDelegate OnDeathLocal;
	
	UPROPERTY(BlueprintAssignable, Category = "Health Handler")
	FOnDeathDelegate OnDeathGlobal;

	UPROPERTY(BlueprintAssignable, Category = "Health Handler")
	FOnDeathDelegate OnDeathServer;

	UPROPERTY(BlueprintAssignable, Category = "Health Handler")
	FHealthDelegate OnInvulnerabilityGlobal;

	UPROPERTY(BlueprintAssignable, Category = "Health Handler")
	FHealthDelegate OnFallDamageReceived;

public:
	
	UPW_HealthComponent();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable) void RecoverHealth(float recoverValue);
	UFUNCTION(BlueprintCallable) void LocalRecoverHealth(float recoverValue);
	UFUNCTION(BlueprintCallable) void SetIsInvulnerable(bool isInvulnerable);
	UFUNCTION(BlueprintCallable) void LocalSetIsInvulnerable(bool isInvulnerable);
	UFUNCTION(BlueprintCallable) void SetCanNaturallyRegenerate(bool canNaturallyRegenerate);
	UFUNCTION(BlueprintCallable) void LocalSetCanNaturallyRegenerate(bool canNaturallyRegenerate);
	UFUNCTION(BlueprintCallable) void ApplyLandedDamage(const FHitResult& hitResult);
	UFUNCTION(BlueprintCallable) void TakeDamage(AActor* OwnerActor, float DamageAmount, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	bool CanReceiveDamage(float damageAmount) const;
	bool CanRecoverHealth();
	bool CanReceiveLandedDamage() const;

	UFUNCTION() void OnRep_HealthChanged(float lastHealth);
	
	FORCEINLINE bool IsAlive() const { return _isAlive; }
	FORCEINLINE bool IsInvulnerable() const { return _isInvulnerable; }
};
