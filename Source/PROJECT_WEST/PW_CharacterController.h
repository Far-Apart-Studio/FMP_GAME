// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW_Weapon.h"
#include "PW_WeaponData.h"
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

	int _currentAmmo = 0;
	int _currentReserveAmmo = 0;
	
	
public:
	APW_CharacterController();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	void CastBulletRay();
	bool CastRay(FVector rayStart, FVector rayDestination, FCollisionQueryParams collisionQueryParams, FHitResult hitResult);
	void EquipWeapon(APW_Weapon* weaponObject);
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;
	void Crouch();
	void MoveForward(float value);
	void MoveRight(float value);
	void ToggleSprint();
	void LookRight(float value);
	void LookUp(float value);
};
