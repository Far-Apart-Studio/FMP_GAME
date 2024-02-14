// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PW_CharacterMovementComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_CharacterMovementComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, Category = "Character")
	class APW_Character* _ownerCharacter;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly,
		Category = "Character", meta = (AllowPrivateAccess = "true"))
	bool _isSprinting = false;

	UPROPERTY(EditAnywhere, Category = "Character")
	float _sprintMultiplier = 1.50f;
	
public:	
	UPW_CharacterMovementComponent();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION() void MoveForward(float value);
	void LocalMoveForward(const FVector& moveDirection, float value);

	UFUNCTION() void MoveRight(float value);
	UFUNCTION() void Jump();
	UFUNCTION() void Sprint();
	UFUNCTION() void LocalSprint();
	
	void AssignInputActions();
	void GetOwnerCharacter();

	UFUNCTION(Server, Reliable)
	void ServerSprint();

	UFUNCTION(Server, Reliable)
	void ServerMoveForward(const FVector& moveDirection, float value);
};
