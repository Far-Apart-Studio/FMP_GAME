// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PW_CharacterController.generated.h"

UCLASS()
class PROJECT_WEST_API APW_CharacterController : public ACharacter
{
	GENERATED_BODY()

private:

	// >>> ------------------ Character Component ------------------ >>> //
	
	UPROPERTY(EditAnywhere, Category = "Character")
	class USceneComponent* _weaponHolder;
	
	UPROPERTY(EditAnywhere, Category = "Character")
	class UCameraComponent* _cameraComponent;

	UPROPERTY(EditAnywhere, Category = "Character")
	float _sprintMultiplier = 1.50f;
	
	bool _isSprinting = false;

	// >>> ------------------ Weapon Handler Component ------------------ >>> //

	UPROPERTY(EditAnywhere, Category = "Weapon Handler")
	class APW_Weapon* _currentWeapon;
	
public:
	APW_CharacterController();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// >>> ------------------ Weapon Handler Component ------------------ >>> //
	
	void CastBulletRay();
	bool CastRay(FVector rayStart, FVector rayDestination, FCollisionQueryParams collisionQueryParams, FHitResult hitResult);
	void ReloadWeapon();
	void AttachDefaultWeapon();
	void FireWeapon();

	FORCEINLINE APW_Weapon* GetCurrentWeapon() const { return _currentWeapon; }
	FORCEINLINE void SetCurrentWeapon(APW_Weapon* currentWeapon) { _currentWeapon = currentWeapon; }

	// >>> ------------------ Character Component ------------------ >>> //
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;
	void Crouch();
	void MoveForward(float value);
	void MoveRight(float value);
	void ToggleSprint();
	void LookRight(float value);
	void LookUp(float value);
};
