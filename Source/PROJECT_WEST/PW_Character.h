// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Items/PW_Item.h"
#include "PW_Character.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftGameDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FShootButtonDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReloadButtonDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEquipButtonDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDropButtonDelegate);

UCLASS()
class PROJECT_WEST_API APW_Character : public ACharacter
{
	GENERATED_BODY()

private:

	// >>> ------------------ Character Component ------------------ >>> //
	
	UPROPERTY(EditAnywhere, Category = "Character")
	class USceneComponent* _weaponHolder;

	UPROPERTY(EditAnywhere, Category = "Character")
	class USceneComponent* _itemHolder;
	
	UPROPERTY(EditAnywhere, Category = "Character")
	class UCameraComponent* _cameraComponent;

	UPROPERTY(EditAnywhere, Category = "Character")
	float _sprintMultiplier = 1.50f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* _overheadWidget;
	
	bool _isSprinting = false;

	// >>> ------------------ Weapon Handler Component ------------------ >>> //


	UPROPERTY(EditAnywhere, Category = "Weapon Handler")
	class UPW_WeaponData* _defaultWeaponData;
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon Handler")
	class APW_Weapon* _currentWeapon;

	UPROPERTY(EditAnywhere, Category = "Weapon Handler")
	float _maximumWeaponFallOffRange = 10000.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon Handler")
	float _lastFiredTime = 0.0f;

	// Move to Item Handler Component
	UPROPERTY(ReplicatedUsing = OnRep_OverlappinItem)
	class APW_Item* _overlappingItem;

	// Move to Item Handler Component
	UPROPERTY(ReplicatedUsing = OnRep_WeaponChange)
	class APW_Item* _itemInHand;
	
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

	FOnLeftGameDelegate _onLeftGameDelegate;
	
public:
	
	APW_Character();
	
	void Elim(bool leftGame);

	UFUNCTION(Server, Reliable)
	void ServerLeaveGame();

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
	void EquipButtonPressed();

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	// >>> ------------------ Weapon Handler Component ------------------ >>> //
	
	void CastBulletRay();
	bool CastRay(const FVector& rayStart, const FVector& rayDestination, const FCollisionQueryParams& collisionQueryParams, FHitResult& hitResult) const;
	void ReloadWeapon();
	void AttachDefaultWeapon();
	void ApplyDamage(const FHitResult& hitResult);
	float CalculateDamage(const FHitResult& hitResult) const;
	bool CalculateFireStatus();
	void FireWeapon();

	FORCEINLINE APW_Weapon* GetCurrentWeapon() const { return _currentWeapon; }
	FORCEINLINE void SetCurrentWeapon(APW_Weapon* currentWeapon) { _currentWeapon = currentWeapon; }
	FORCEINLINE UWidgetComponent* GetOverheadWidget() const { return _overheadWidget; }

	// >>> ------------------ Character Component ------------------ >>> //
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;
	void Crouch();
	void MoveForward(float value);
	void MoveRight(float value);
	void ToggleSprint();
	void LookRight(float value);
	void LookUp(float value);
	
	void SetOverlappingItem(class APW_Item* Item);
	void EquipItem(APW_Item* Apw_Item);
	void DropItem();
	void DropButtonPressed();
};
