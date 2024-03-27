// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW_HealthComponent.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PW_CharacterMovementComponent.generated.h"

USTRUCT(BlueprintType)
struct FDashData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	float DashSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	float DashDuration = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	float DashCooldown = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	UCurveFloat* DashCurve;

	bool CanDash = true;
	FTimerHandle DashCooldownTimer;
};

USTRUCT(BlueprintType)
struct FStaminaData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	float CurrentStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	float MinimumStamina = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	float StaminaRecoveryRate = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
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
	float _dashStaminaCost = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	FDashData _dashData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	FStaminaData _staminaData;

	UPROPERTY(EditAnywhere, Category = "Character", meta = (AllowPrivateAccess = "true"))
	FRegenerationHandle _staminaRegenerationHandle;

	UPROPERTY(EditAnywhere, Category = "Character", meta = (AllowPrivateAccess = "true"))
	FRegenerationHandle _staminaReductionHandle;

	UPROPERTY(VisibleAnywhere, Category = "Character")
	bool _canMove;

public:
	
	UPROPERTY(BlueprintAssignable, Category = "Character")
	FOnCompleteDelegate OnDashLocal;
	
	UPROPERTY(BlueprintAssignable, Category = "Character")
	FOnCompleteDelegate OnDashServer;
	
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
	UFUNCTION() void CompleteDash();
	
	UFUNCTION() void Dash();
	void LocalDash(FVector dashDirection, const FVector& defaultDashDirection);
	UFUNCTION(Server, Reliable) void ServerDash(FVector dashDirection, FVector defaultDashDirection);
	
	UFUNCTION() void BeginSprint();
	void LocalBeginSprint();
	UFUNCTION(Server, Reliable) void ServerBeginSprint();
	
	UFUNCTION() void CancelSprint();
	void LocalCancelSprint();
	UFUNCTION(Server, Reliable) void ServerCancelSprint();

	void AssignInputActions();
	void GetOwnerCharacter();
	bool ShouldReduceStamina();
	bool ShouldIncreaseStamina();
	void CompleteDashCooldown();
	bool CanDash(const UCharacterMovementComponent* characterMovement);

	FORCEINLINE void SetCanMove(bool value) { _canMove = value; }
};
