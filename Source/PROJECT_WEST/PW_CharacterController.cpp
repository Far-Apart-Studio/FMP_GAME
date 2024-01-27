// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_CharacterController.h"

#include "PW_Utilities.h"
#include "PW_Weapon.h"
#include "PW_WeaponData.h"
#include "PW_WeaponHandlerComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

APW_CharacterController::APW_CharacterController()
{
	PrimaryActorTick.bCanEverTick = true;
	
	_cameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	_cameraComponent->SetupAttachment(RootComponent);
	_cameraComponent->bUsePawnControlRotation = true;

	_weaponHolder = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponHolder"));
	_weaponHolder->SetupAttachment(RootComponent);
}

void APW_CharacterController::BeginPlay()
{
	Super::BeginPlay();
}

void APW_CharacterController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// >>> ------------------ Weapon Handler Component ------------------ >>> //

void APW_CharacterController::CastBulletRay()
{
	FVector rayDirection = _cameraComponent->GetForwardVector();
	FVector rayStart = _cameraComponent->GetComponentLocation();
	FVector rayDestination = rayStart + (rayDirection * 10000.0f);
	
	FCollisionQueryParams collisionQueryParams;
	collisionQueryParams.AddIgnoredActor(this);
	FHitResult hitResult;

	bool isActorHit = CastRay(rayStart, rayDestination, collisionQueryParams, hitResult);

	// Debug >>>
	if (isActorHit)
		DrawDebugLine(GetWorld(), rayStart, rayDestination, FColor::Green,
			false, 2.0f, 0, 2.0f);
	else
		DrawDebugLine(GetWorld(), rayStart, rayDestination, FColor::Red,
			false, 2.0f, 0, 2.0f);
	// Debug <<<
}

bool APW_CharacterController::CastRay(FVector rayStart, FVector rayDestination,
	FCollisionQueryParams collisionQueryParams, FHitResult hitResult)
{
	GetWorld()->LineTraceSingleByChannel(hitResult, rayStart, rayDestination,
		ECC_Visibility, collisionQueryParams);
	
	if (!hitResult.bBlockingHit)
	{
		hitResult.Location = rayDestination;
		return false;
	}
	return true;
}

void APW_CharacterController::EquipWeapon(APW_Weapon* weaponObject)
{
	UPW_WeaponData* weaponData = weaponObject->GetWeaponData();
	
}

// <<< ------------------ Weapon Handler Component ------------------ <<< //


void APW_CharacterController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APW_CharacterController::Jump);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APW_CharacterController::Crouch);
	PlayerInputComponent->BindAction("SprintToggle", IE_Pressed, this, &APW_CharacterController::ToggleSprint);
	PlayerInputComponent->BindAxis("MoveForward", this, &APW_CharacterController::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APW_CharacterController::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &APW_CharacterController::LookRight);
	PlayerInputComponent->BindAxis("LookRight", this, &APW_CharacterController::LookUp);
}

void APW_CharacterController::MoveForward(float value)
{
	const FVector moveDirection = GetActorForwardVector();
	AddMovementInput(moveDirection, value);
}

void APW_CharacterController::MoveRight(float value)
{
	const FVector moveDirection = GetActorRightVector();
	AddMovementInput(moveDirection, value);
}

void APW_CharacterController::ToggleSprint()
{
	_isSprinting = !_isSprinting;
	
	_isSprinting ?
		GetCharacterMovement()->MaxWalkSpeed *= _sprintMultiplier :
		GetCharacterMovement()->MaxWalkSpeed /= _sprintMultiplier;
}

void APW_CharacterController::LookRight(float value)
{
	AddControllerYawInput(value);
}

void APW_CharacterController::LookUp(float value)
{
	AddControllerPitchInput(value);
}

void APW_CharacterController::Jump()
{
	Super::Jump();
}

void APW_CharacterController::Crouch()
{
	Super::Crouch();
	UE_LOG(LogTemp, Warning, TEXT("Crouch"));
	CastBulletRay();
}







