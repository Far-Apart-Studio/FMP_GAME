// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Items/PW_Item.h"
#include "PW_Character.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FButtonPressedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAxisModifiedDelegate, float, value);

UCLASS()
class PROJECT_WEST_API APW_Character : public ACharacter
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, Category = "Character")
	USceneComponent* _itemHolder;
	
	UPROPERTY(EditAnywhere, Category = "Character")
	class UCameraComponent* _cameraComponent;

	UPROPERTY(VisibleAnywhere, Category = "Character")
	class UPW_ItemHandlerComponent* _itemHandlerComponent;

	UPROPERTY(VisibleAnywhere, Category = "Character")
	class UPW_HealthComponent* _healthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* _overheadWidget;

	class APW_PlayerController* _playerController;
	class APW_BountyGameMode* _bountyGameMode;
	bool _LeftGame = false;
public:
	
	APW_Character();

	FButtonPressedDelegate OnShootButtonPressed;
	FButtonPressedDelegate OnShootReleaseDelegate;
	FButtonPressedDelegate OnReloadButtonPressed;

	FButtonPressedDelegate OnPickUpButtonPressed;
	FButtonPressedDelegate OnDropButtonPressed;
	FButtonPressedDelegate OnSwitchItemButtonPressed;
	FButtonPressedDelegate OnLeftGameDelegate;
	FButtonPressedDelegate OnStartInteractButtonPressed;
	FButtonPressedDelegate OnEndInteractButtonPressed;

	FButtonPressedDelegate OnJumpButtonPressed;
	FButtonPressedDelegate OnCrouchButtonPressed;
	FButtonPressedDelegate OnSprintButtonPressed;
	
	FAxisModifiedDelegate OnMoveForwardAxisPressed;
	FAxisModifiedDelegate OnMoveRightAxisPressed;
	
	void Elim(bool leftGame);

	UFUNCTION(Server, Reliable)
	void ServerLeaveGame();

protected:

	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnDeath(AActor* DamageCauser, AController* DamageCauserController);
	
	UFUNCTION()
	void OnHealthChanged();

	UFUNCTION( NetMulticast, Reliable )
	void MultiCastElim (bool leftGame);
	
	virtual void PostInitializeComponents() override;
	void PickUpButtonPressed();
	void SwitchItemButtonPressed();
	
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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
	void ToggleMovement(bool canMove);
	void StartInteractButtonPressed();
	void EndInteractButtonPressed();
	
	UFUNCTION(BlueprintCallable)
	bool IsAlive() const;
	
public:
	FORCEINLINE USceneComponent* GetItemHolder() const { return _itemHolder; }
	FORCEINLINE UCameraComponent* GetCameraComponent() const { return _cameraComponent; }
};
