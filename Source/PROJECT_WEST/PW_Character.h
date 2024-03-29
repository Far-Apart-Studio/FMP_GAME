// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PW_Character.generated.h"

class APW_PlayerController;
class APW_BountyGameMode;
class UCameraComponent;
class UPW_ItemHandlerComponent;
class UPW_HealthComponent;
class USpringArmComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FButtonPressedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FButtonToggleDelegate, bool, value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAxisModifiedDelegate, float, value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSlotPressedDelegate, int, slotIndex);

UCLASS()
class PROJECT_WEST_API APW_Character : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Character")
	USceneComponent* _itemHolder;
	
	UPROPERTY(EditAnywhere, Category = "Character")
	UCameraComponent* _cameraComponent;

	UPROPERTY(VisibleAnywhere, Category = "Character")
	UPW_HealthComponent* _healthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* _overheadWidget;

	UPROPERTY(VisibleAnywhere, Category = "Character")
	USpringArmComponent* _springArmComponent;

	UPROPERTY(EditAnywhere, Category = "Character")
	float _sprintDoublePressTime = 0.2f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess = "true"))
	APW_PlayerController* _playerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess = "true"))
	APW_BountyGameMode* _bountyGameMode;
	
	class UPW_CharacterMovementComponent* _characterMovementComponent;
	
	bool _LeftGame = false;
	bool _isPressedSprint = false;
	FTimerHandle _sprintTimerHandle;
	FTimerDelegate _sprintTimerDelegate;
	bool _canLook;
	
public:
	
	APW_Character();

	UPROPERTY(BlueprintAssignable)
	FButtonPressedDelegate OnAimButtonPressed;

	UPROPERTY(BlueprintAssignable)
	FButtonPressedDelegate OnAimButtonReleased;
	
	UPROPERTY(BlueprintAssignable)
	FButtonPressedDelegate OnShootButtonPressed;

	UPROPERTY(BlueprintAssignable)
	FButtonPressedDelegate OnShootButtonReleased;

	UPROPERTY(BlueprintAssignable)
	FButtonPressedDelegate OnReloadButtonPressed;

	UPROPERTY(BlueprintAssignable)
	FButtonPressedDelegate OnDropButtonPressed;

	UPROPERTY(BlueprintAssignable)
	FButtonPressedDelegate OnCycleItemButtonPressed;

	UPROPERTY(BlueprintAssignable)
	FButtonPressedDelegate OnLeftGameDelegate;

	UPROPERTY(BlueprintAssignable)
	FButtonPressedDelegate OnDashButtonPressed;

	UPROPERTY(BlueprintAssignable)
	FButtonPressedDelegate OnInteractButtonPressed;

	UPROPERTY(BlueprintAssignable)
	FButtonPressedDelegate OnEndInteractButtonPressed;

	UPROPERTY(BlueprintAssignable)
	FButtonPressedDelegate OnJumpButtonPressed;

	UPROPERTY(BlueprintAssignable)
	FButtonPressedDelegate OnCrouchButtonPressed;

	UPROPERTY(BlueprintAssignable)
	FButtonPressedDelegate OnSprintButtonPressed;

	UPROPERTY(BlueprintAssignable)
	FButtonPressedDelegate OnSprintButtonReleased;

	UPROPERTY(BlueprintAssignable)
	FAxisModifiedDelegate OnMoveForwardAxisPressed;

	UPROPERTY(BlueprintAssignable)
	FAxisModifiedDelegate OnMoveRightAxisPressed;

	UPROPERTY(BlueprintAssignable)
	FButtonPressedDelegate OnMouseUpPressed;

	UPROPERTY(BlueprintAssignable)
	FButtonPressedDelegate OnMouseDownPressed;

	UPROPERTY(BlueprintAssignable)
	FSlotPressedDelegate OnSlotPressed;

	UPROPERTY(BlueprintAssignable)
	FButtonPressedDelegate OnCameraRotationChange;
	
	UPROPERTY(BlueprintAssignable)
	FButtonToggleDelegate OnInteractButtonToggled;
	
	void Elim(bool leftGame);

	UFUNCTION(Server, Reliable)
	void ServerLeaveGame();

protected:

	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnDeath(AActor* OwnerActor, AActor* DamageCauser, AController* DamageCauserController);
	
	UFUNCTION()
	void OnHealthChanged();

	UFUNCTION( NetMulticast, Reliable )
	void MultiCastElim (bool leftGame);
	
	virtual void PostInitializeComponents() override;
	void InteractButtonPressed();
	void SwitchItemButtonPressed();
	
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ReloadButtonPressed();
	void SecondaryUseButtonPressed();
	void SecondaryUseButtonReleased();
	void JumpButtonPressed();
	void UseButtonPressed();
	void UseButtonReleased();
	void CrouchButtonPressed();
	void MoveForwardAxisPressed(float value);
	void MoveRightAxisPressed(float value);
	void LookRightAxisPressed(float value);
	void LookUpAxisPressed(float value);
	void DropButtonPressed();
	void SprintButtonPressed();
	void SprintButtonReleased();
	void ToggleMovement(bool canMove);
	void InteractButtonReleased();
	void ReturnedButtonPressed();
	void MouseWheelUpAxisPressed();
	void MouseWheelDownAxisPressed();
	void SlotOnePressed();
	void SlotTwoPressed();
	void SlotThreePressed();
	void SlotFourPressed();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void StopCrouching();
	
	UFUNCTION(BlueprintCallable)
	bool IsAlive() const;
	
public:
	FORCEINLINE USceneComponent* GetItemHolder() const { return _itemHolder; }
	FORCEINLINE UCameraComponent* GetCameraComponent() const { return _cameraComponent; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetItemHolderComponent(USceneComponent* itemHolderComponent) { _itemHolder = itemHolderComponent; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetCameraComponent(UCameraComponent* cameraComponent) { _cameraComponent = cameraComponent; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetSpringArmComponent(USpringArmComponent* springArmComponent) { _springArmComponent = springArmComponent; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetWidgetComponent(UWidgetComponent* widgetComponent) { _overheadWidget = widgetComponent; }
};
