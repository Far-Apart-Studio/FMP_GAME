// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_CharacterMovementComponent.h"
#include "FDashAction.h"
#include "FRecoilAction.h"
#include "PW_Character.h"
#include "PW_Utilities.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

UPW_CharacterMovementComponent::UPW_CharacterMovementComponent():
	_ownerCharacter(nullptr), _dashData()
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

void UPW_CharacterMovementComponent::Dash()
{
	if (_ownerCharacter == nullptr)
		{ PW_Utilities::Log("OWNER CHARACTER NOT FOUND!"); return; }

	const UCharacterMovementComponent* characterMovement = _ownerCharacter->GetCharacterMovement();

	if (characterMovement == nullptr)
		{ PW_Utilities::Log("CHARACTER MOVEMENT NOT FOUND!"); return; }

	if (!CanDash(characterMovement))
		return;

	_dashData.CanDash = false;
	OnDash.Broadcast();
	
	FLatentActionInfo latentActionInfo = FLatentActionInfo();
	latentActionInfo.CallbackTarget = this;
	latentActionInfo.ExecutionFunction = "CompleteDash";

	const float moveRight = _ownerCharacter->GetInputAxisValue("MoveRight");
	const float moveForward = _ownerCharacter->GetInputAxisValue("MoveForward");

	const FVector forwardDirection = _ownerCharacter->GetActorForwardVector() * moveForward;
	const FVector rightDirection = _ownerCharacter->GetActorRightVector() * moveRight;
	FVector dashDirection = forwardDirection + rightDirection;

	if (dashDirection.Size() == 0.0f)
		dashDirection = _ownerCharacter->GetActorForwardVector();

	const FGuid UUID = FGuid::NewGuid();
	TUniquePtr<FDashAction> dashAction = MakeUnique<FDashAction>
		(_dashData.DashDuration, _dashData.DashSpeed, dashDirection,
			_dashData.DashCurve, _ownerCharacter, latentActionInfo);

	FLatentActionManager& latentActionManager = GetWorld()->GetLatentActionManager();
	latentActionManager.AddNewAction(this, UUID.A, dashAction.Get());
	
	GetWorld()->GetTimerManager().SetTimer(_dashData.DashCooldownTimer, this,
		&UPW_CharacterMovementComponent::CompleteDashCooldown, _dashData.DashCooldown, false);
	
	dashAction.Release();
}

void UPW_CharacterMovementComponent::CompleteDash()
{
	OnDashComplete.Broadcast();
}

void UPW_CharacterMovementComponent::CompleteDashCooldown()
{
	OnDashCooldownComplete.Broadcast();
	_dashData.CanDash = true;
}

bool UPW_CharacterMovementComponent::CanDash(const UCharacterMovementComponent* characterMovement)
{
	return characterMovement->IsWalking()
	&& _dashData.CanDash
	&& _ownerCharacter->IsLocallyControlled();
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

	_ownerCharacter->OnSprintButtonReleased.AddDynamic
		(this, &UPW_CharacterMovementComponent::Sprint);

	_ownerCharacter->OnDashButtonPressed.AddDynamic
		(this, &UPW_CharacterMovementComponent::Dash);
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
