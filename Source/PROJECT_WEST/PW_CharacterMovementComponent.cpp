// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_CharacterMovementComponent.h"

#include "DebugMacros.h"
#include "PW_Character.h"
#include "PW_Utilities.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

UPW_CharacterMovementComponent::UPW_CharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPW_CharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	SetIsReplicated(true);
	GetOwnerCharacter();

	if (_ownerCharacter->IsLocallyControlled())
		AssignInputActions();
}

void UPW_CharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPW_CharacterMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPW_CharacterMovementComponent, _isSprinting);
}

void UPW_CharacterMovementComponent::MoveForward(float value)
{
	const FVector moveDirection = _ownerCharacter->GetActorForwardVector();
	if (_ownerCharacter->IsLocallyControlled() || _ownerCharacter->HasAuthority())
		_ownerCharacter->AddMovementInput(moveDirection, value);
}


void UPW_CharacterMovementComponent::MoveRight(float value)
{
	const FVector moveDirection = _ownerCharacter->GetActorRightVector();
	_ownerCharacter->AddMovementInput(moveDirection, value);
}

void UPW_CharacterMovementComponent::Jump()
{
	_ownerCharacter->Jump();
}

void UPW_CharacterMovementComponent::Sprint()
{
	_ownerCharacter->HasAuthority() ? LocalSprint() : ServerSprint();
}

void UPW_CharacterMovementComponent::LocalSprint()
{	
	_isSprinting = !_isSprinting;
	_isSprinting ?
		_ownerCharacter->GetCharacterMovement()->MaxWalkSpeed *= _sprintMultiplier :
		_ownerCharacter->GetCharacterMovement()->MaxWalkSpeed /= _sprintMultiplier;
}

void UPW_CharacterMovementComponent::ServerSprint_Implementation()
{
	if (_ownerCharacter->HasAuthority())
		LocalSprint();
}

void UPW_CharacterMovementComponent::AssignInputActions()
{
	_ownerCharacter->OnMoveForwardAxisPressed.AddDynamic
		(this, &UPW_CharacterMovementComponent::MoveForward);
	
	_ownerCharacter->OnMoveRightAxisPressed.AddDynamic
		(this, &UPW_CharacterMovementComponent::MoveRight);
	
	_ownerCharacter->OnJumpButtonPressed.AddDynamic
		(this, &UPW_CharacterMovementComponent::Jump);
	
	_ownerCharacter->OnSprintButtonPressed.AddDynamic
		(this, &UPW_CharacterMovementComponent::Sprint);
}

void UPW_CharacterMovementComponent::GetOwnerCharacter()
{
	AActor* ownerActor = GetOwner();
	
	if (ownerActor == nullptr)
		{ PW_Utilities::Log("OWNER ACTOR NOT FOUND!"); return; }

	_ownerCharacter = Cast<APW_Character>(ownerActor);
	
	if (_ownerCharacter == nullptr)
		{ PW_Utilities::Log("OWNER CHARACTER NOT FOUND!"); }
}

void UPW_CharacterMovementComponent::ServerMoveForward_Implementation(const FVector& moveDirection, float value)
{
	
}
