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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDropButtonDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEquipButtonDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FShootButtonDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReloadButtonDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftGameDelegate);

UCLASS()
class PROJECT_WEST_API APW_Character : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Character")
	USceneComponent* _objectHolder;

	UPROPERTY(EditAnywhere, Category = "Character")
	USceneComponent* _itemHolder;
	
	UPROPERTY(EditAnywhere, Category = "Character")
	class UCameraComponent* _cameraComponent;

	UPROPERTY(EditAnywhere, Category = "Character")
	float _sprintMultiplier = 1.50f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* _overheadWidget;
	
	bool _isSprinting = false;
	
	// >>> ------------------ Item Handler Component ------------------ >>> //
	UPROPERTY(ReplicatedUsing = OnRep_OverlappinItem)
	class APW_Item* _overlappingItem;
	
	UPROPERTY(ReplicatedUsing = OnRep_WeaponChange)
	class APW_Item* _itemInHand;
	// <<< ------------------ Item Handler Component ------------------ <<< //
	
	UFUNCTION()
	void OnRep_WeaponChange(APW_Item* LastWeapon);
	
	UFUNCTION()
	void OnRep_OverlappinItem(APW_Item* LastWeapon);

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	UFUNCTION(Server, Reliable)
	void ServerDropButtonPressed();

	bool _LeftGame = false;

	UFUNCTION( NetMulticast, Reliable )
	void MultiCastElim (bool leftGame);

	
public:
	
	APW_Character();

	FShootButtonDelegate OnShootButtonPressed;
	FReloadButtonDelegate OnReloadButtonPressed;
	FOnLeftGameDelegate OnLeftGameDelegate;
	
	void Elim(bool leftGame);

	UFUNCTION(Server, Reliable)
	void ServerLeaveGame();

protected:

	virtual void BeginPlay() override;
	void EquipButtonPressed();

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
	void SetOverlappingItem(class APW_Item* Item);
	void EquipItem(APW_Item* Apw_Item);
	void DropItem();
	void DropButtonPressed();

public:
	FORCEINLINE USceneComponent* GetObjectHolder() const { return _objectHolder; }
	FORCEINLINE UCameraComponent* GetCameraComponent() const { return _cameraComponent; }
	
};
