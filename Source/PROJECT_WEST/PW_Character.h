// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Items/PW_Item.h"
#include "PW_Character.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJumpButtonDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCrouchButtonDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSprintButtonDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMoveForwardAxisDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMoveRightAxisDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLookRightAxisDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLookUpAxisDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPickUpButtonDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDropButtonDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSwitchItemButtonDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FShootButtonDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReloadButtonDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftGameDelegate);

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

	UPROPERTY(EditAnywhere, Category = "Character")
	float _sprintMultiplier = 1.50f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* _overheadWidget;

	class APW_PlayerController* _playerController;
	class APW_BountyGameMode* _bountyGameMode;
	
	bool _isSprinting = false;
	
	bool _LeftGame = false;

	UFUNCTION( NetMulticast, Reliable )
	void MultiCastElim (bool leftGame);

	
public:
	
	APW_Character();

	FShootButtonDelegate OnShootButtonPressed;
	FReloadButtonDelegate OnReloadButtonPressed;

	FPickUpButtonDelegate OnPickUpButtonPressed;
	FDropButtonDelegate OnDropButtonPressed;
	FSwitchItemButtonDelegate OnSwitchItemButtonPressed;
	
	FOnLeftGameDelegate OnLeftGameDelegate;
	
	void Elim(bool leftGame);

	UFUNCTION(Server, Reliable)
	void ServerLeaveGame();

protected:

	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnDeath(AActor* DamageCauser, AController* DamageCauserController);
	
	UFUNCTION()
	void OnHealthChanged();
	
	virtual void PostInitializeComponents() override;
	void PickUpButtonPressed();
	void SwitchItemButtonPressed();
	
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void JumpButtonPressed();
	void UseButtonPressed();
	void CrouchButtonPressed();
	void MoveForwardAxisPressed(float value);
	void MoveRightAxisPressed(float value);
	void SprintButtonPressed();
	void LookRightAxisPressed(float value);
	void LookUpAxisPressed(float value);
	void DropButtonPressed();
	
	UFUNCTION(BlueprintCallable)
	bool IsAlive() const;
	
public:
	FORCEINLINE USceneComponent* GetItemHolder() const { return _itemHolder; }
	FORCEINLINE UCameraComponent* GetCameraComponent() const { return _cameraComponent; }
};
