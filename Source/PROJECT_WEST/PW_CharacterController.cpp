// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_CharacterController.h"

#include "PW_Utilities.h"
#include "PW_Weapon.h"
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

	FTimerHandle timerHandle;
	GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &APW_CharacterController::AttachDefaultWeapon, 5.0f, false);
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

void APW_CharacterController::FireWeapon()
{
	if (_currentWeapon == nullptr)
		{ PW_Utilities::Log("NO CURRENT WEAPON EQUIPPED!"); return; }

	if (_currentWeapon->IsAmmoEmpty())
	{
		ReloadWeapon();
		return;
	}

	CastBulletRay();
	_currentWeapon->SubtractCurrentAmmo(1);
} 

void APW_CharacterController::ReloadWeapon()
{
	if (_currentWeapon == nullptr)
		{ PW_Utilities::Log("NO CURRENT WEAPON EQUIPPED!"); return; }
	
	if (_currentWeapon->IsReloading())
		return;
	
	_currentWeapon->SetReloading(true);

	const UPW_WeaponData* weaponData = _currentWeapon->GetWeaponData();

	if (weaponData == nullptr)
		{ PW_Utilities::Log("NO WEAPON DATA FOUND!"); return; }
	
	const float reloadTime = weaponData->GetWeaponReloadTime();
	
	FTimerHandle reloadTimerHandle;
	FTimerDelegate reloadTimerDelegate;
	reloadTimerDelegate.BindLambda([this]
	{
		_currentWeapon->SetReloading(false);
		_currentWeapon->TransferReserveAmmo();
	});

	GetWorldTimerManager().SetTimer(reloadTimerHandle, reloadTimerDelegate, reloadTime, false);
}

//Debug Function <<<
void APW_CharacterController::AttachDefaultWeapon()
{
	UWorld* currentWorld = GetWorld();
	FActorSpawnParameters spawnParameters;
	spawnParameters.Owner = this;
	spawnParameters.Instigator = this;

	const FVector spawnLocation = _weaponHolder->GetComponentLocation();
	const FRotator spawnRotation = _weaponHolder->GetComponentRotation();

	_currentWeapon = currentWorld->SpawnActor<APW_Weapon>(spawnLocation, spawnRotation, spawnParameters);

	if (_currentWeapon == nullptr)
		{ PW_Utilities::Log("DEFAULT WEAPON NOT FOUND!"); return; }
	
	SetCurrentWeapon(_currentWeapon);
	_currentWeapon->AttachToComponent(_weaponHolder, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	_currentWeapon->InitialiseWeapon(_defaultWeaponData);
}
// Debug Function >>>

// <<< ------------------ Weapon Handler Component ------------------ <<< //


void APW_CharacterController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APW_CharacterController::Jump);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APW_CharacterController::Crouch);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APW_CharacterController::FireWeapon);
	
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
}







