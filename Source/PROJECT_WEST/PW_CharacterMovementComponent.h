// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PW_CharacterMovementComponent.generated.h"

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
	float _dashSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Character")
	float _dashDuration = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Character")
	float _dashCooldown = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Character")
	UCurveFloat* _dashCurve;

	bool _canDash = true;
	FTimerHandle _dashCooldownTimer;

public:
	UPROPERTY(BlueprintAssignable, Category = "Character")
	FOnCompleteDelegate OnDash;
	
	UPROPERTY(BlueprintAssignable, Category = "Character")
	FOnCompleteDelegate OnDashComplete;

	UPROPERTY(BlueprintAssignable, Category = "Character")
	FOnCompleteDelegate OnDashCooldownComplete;
	
public:	


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

	UFUNCTION(Server, Reliable)
	void ServerSprint();
};
