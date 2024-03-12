// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW_HealthComponent.h"
#include "RegenerationHandle.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PW_CharacterMovementComponent.generated.h"

USTRUCT()
struct FDashData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Character")
	float DashSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Character")
	float DashDuration = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Character")
	float DashCooldown = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Character")
	UCurveFloat* DashCurve;

	bool CanDash = true;
	FTimerHandle DashCooldownTimer;
};

USTRUCT()
struct FStaminaData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Character")
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Character")
	float CurrentStamina = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Character")
	float MinimumStamina = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Character")
	float StaminaRecoveryRate = 1.0f;
	
	UPROPERTY(EditAnywhere, Category = "Character")
	float StaminaRecoveryAmount = 10.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCompleteDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_CharacterMovementComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, Category = "Character")
	class APW_Character* _ownerCharacter;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	bool _isSprinting = false;

	UPROPERTY(EditAnywhere, Category = "Character")
	float _sprintMultiplier = 1.50f;

	UPROPERTY(EditAnywhere, Category = "Character")
	FDashData _dashData;

	UPROPERTY(EditAnywhere, Category = "Character")
	float _dashStaminaCost = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Character")
	FStaminaData _staminaData;

	UPROPERTY(EditAnywhere, Category = "Character")
	FRegenerationHandle _staminaRegenerationHandle;

public:
	UPROPERTY(BlueprintAssignable, Category = "Character")
	FOnCompleteDelegate OnDash;
	
	UPROPERTY(BlueprintAssignable, Category = "Character")
	FOnCompleteDelegate OnDashComplete;

	UPROPERTY(BlueprintAssignable, Category = "Character")
	FOnCompleteDelegate OnDashCooldownComplete;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	UPW_CharacterMovementComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION() void MoveForward(float value);
	UFUNCTION() void MoveRight(float value);
	UFUNCTION() void Jump();
	UFUNCTION() void Dash();
	UFUNCTION() void CompleteDash();
	UFUNCTION() void Sprint();
	UFUNCTION() void LocalSprint();
	
	void AssignInputActions();
	void GetOwnerCharacter();
	void CompleteDashCooldown();
	bool CanDash(const UCharacterMovementComponent* characterMovement);

	UFUNCTION(Server, Reliable) void ServerSprint();
};
