// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_CharacterController.h"
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
}







