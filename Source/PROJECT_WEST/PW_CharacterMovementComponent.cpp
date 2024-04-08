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
	_canMove = true;
}

void UPW_CharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	SetIsReplicated(true);
	GetOwnerCharacter();

	if (_ownerCharacter->IsLocallyControlled())
	{
		AssignInputActions();

		TWeakObjectPtr<UPW_CharacterMovementComponent> weakThis = this;
		
		_staminaReductionHandle.RegenerationCondition.BindLambda([weakThis, this]()
			{ return weakThis.IsValid() && weakThis->ShouldReduceStamina();  });

		_staminaReductionHandle.OnReachMinimum.BindLambda([weakThis]()
			{ if (weakThis.IsValid()) weakThis->CancelSprint(); });
		
		_staminaRegenerationHandle.RegenerationCondition.BindLambda([weakThis]()
			{ return weakThis.IsValid() && weakThis->ShouldIncreaseStamina();  });
	}
}

void UPW_CharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	_staminaRegenerationHandle.Increase(_staminaData.CurrentStamina, DeltaTime);
	_staminaReductionHandle.Increase(_staminaData.CurrentStamina, DeltaTime);
}

void UPW_CharacterMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPW_CharacterMovementComponent, _isSprinting);
	DOREPLIFETIME(UPW_CharacterMovementComponent, _replicatedMovementData);
}

void UPW_CharacterMovementComponent::MoveForward(float value)
{
	if (!_canMove)
		return;
	
	const FVector moveDirection = _ownerCharacter->GetActorForwardVector();
	if (_ownerCharacter->IsLocallyControlled() || _ownerCharacter->HasAuthority())
		_ownerCharacter->AddMovementInput(moveDirection, value);
}

void UPW_CharacterMovementComponent::MoveRight(float value)
{
	if (!_canMove)
		return;
	
	const FVector moveDirection = _ownerCharacter->GetActorRightVector();
	if (_ownerCharacter->IsLocallyControlled() || _ownerCharacter->HasAuthority())
		_ownerCharacter->AddMovementInput(moveDirection, value);
}

void UPW_CharacterMovementComponent::Jump()
{
	_ownerCharacter->Jump();
}

void UPW_CharacterMovementComponent::Dash()
{
	if (_ownerCharacter == nullptr)
	{
	//PW_Utilities::Log("OWNER CHARACTER NOT FOUND!"); return;
	}

	const UCharacterMovementComponent* characterMovement = _ownerCharacter->GetCharacterMovement();

	if (characterMovement == nullptr)
	{
	//PW_Utilities::Log("CHARACTER MOVEMENT NOT FOUND!"); return;
	}

	if (!CanDash(characterMovement))
		return;

	OnDashLocal.Broadcast();
	_dashData.CanDash = false;

	const float moveRight = _ownerCharacter->GetInputAxisValue("MoveRight");
	const float moveForward = _ownerCharacter->GetInputAxisValue("MoveForward");

	const FVector forwardDirection = _ownerCharacter->GetActorForwardVector() * moveForward;
	const FVector rightDirection = _ownerCharacter->GetActorRightVector() * moveRight;
	const FVector dashDirection = forwardDirection + rightDirection;

	GetWorld()->GetTimerManager().SetTimer(_dashData.DashCooldownTimer, this,
		&UPW_CharacterMovementComponent::CompleteDashCooldown, _dashData.DashCooldown, false);
	
	GetOwnerRole() == ROLE_Authority ? LocalDash(dashDirection, forwardDirection)
		: ServerDash(dashDirection, forwardDirection);
}

void UPW_CharacterMovementComponent::LocalDash(FVector dashDirection, const FVector& defaultDashDirection)
{
	OnDashServer.Broadcast();
	
	FLatentActionInfo latentActionInfo = FLatentActionInfo();
	latentActionInfo.CallbackTarget = this;
	latentActionInfo.ExecutionFunction = "CompleteDash";

	if (dashDirection.Size() == 0.0f)
		dashDirection = defaultDashDirection;

	const FGuid UUID = FGuid::NewGuid();
	TUniquePtr<FDashAction> dashAction = MakeUnique<FDashAction>
		(_dashData.DashDuration, _dashData.DashSpeed, dashDirection,
			_dashData.DashCurve, _ownerCharacter, latentActionInfo);

	FLatentActionManager& latentActionManager = GetWorld()->GetLatentActionManager();
	latentActionManager.AddNewAction(this, UUID.A, dashAction.Get());
	
	dashAction.Release();
}

void UPW_CharacterMovementComponent::ServerDash_Implementation(FVector dashDirection, FVector defaultDashDirection)
{
	if (GetOwnerRole() == ROLE_Authority)
		LocalDash(dashDirection, defaultDashDirection);
}

void UPW_CharacterMovementComponent::CompleteDash()
{
	OnDashComplete.Broadcast();
	_staminaData.CurrentStamina -= _dashStaminaCost;
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
	&& _ownerCharacter->IsLocallyControlled()
	&& _staminaData.CurrentStamina >= _dashStaminaCost;
}

void UPW_CharacterMovementComponent::BeginSprint()
{
	if (_isSprinting)
		return;
	
	_isSprinting = true;
	GetOwnerRole() == ROLE_Authority ? LocalBeginSprint() : ServerBeginSprint();
}

void UPW_CharacterMovementComponent::LocalBeginSprint()
{
	if (_sprintMultiplier < 0.0f)
		return;
	
	_ownerCharacter->GetCharacterMovement()->MaxWalkSpeed *= _sprintMultiplier;
	const float maximumVelocity = _ownerCharacter->GetCharacterMovement()->MaxWalkSpeed;

	_replicatedMovementData.IsSprinting = true;
	_replicatedMovementData.MaximumVelocity = maximumVelocity;
	_replicatedMovementData.SprintMultiplier = _sprintMultiplier;
}

void UPW_CharacterMovementComponent::ServerBeginSprint_Implementation()
{
	if (GetOwnerRole() == ROLE_Authority)
		LocalBeginSprint();
}


void UPW_CharacterMovementComponent::CancelSprint()
{
	if (!_isSprinting)
		return;
	
	_isSprinting = false;
	
	GetOwnerRole() == ROLE_Authority ? LocalCancelSprint() : ServerCancelSprint();
}

void UPW_CharacterMovementComponent::LocalCancelSprint()
{
	if (_sprintMultiplier < 0.0f)
		return;
	
	_ownerCharacter->GetCharacterMovement()->MaxWalkSpeed /= _sprintMultiplier;
	const float maximumVelocity = _ownerCharacter->GetCharacterMovement()->MaxWalkSpeed;

	_replicatedMovementData.IsSprinting = false;
	_replicatedMovementData.MaximumVelocity = maximumVelocity;
	_replicatedMovementData.SprintMultiplier = 1.0f;
}

void UPW_CharacterMovementComponent::ServerCancelSprint_Implementation()
{
	if (GetOwnerRole() == ROLE_Authority)
		LocalCancelSprint();
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
		(this, &UPW_CharacterMovementComponent::BeginSprint);

	_ownerCharacter->OnSprintButtonReleased.AddDynamic
		(this, &UPW_CharacterMovementComponent::CancelSprint);

	_ownerCharacter->OnDashButtonPressed.AddDynamic
		(this, &UPW_CharacterMovementComponent::Dash);
}

void UPW_CharacterMovementComponent::GetOwnerCharacter()
{
	AActor* ownerActor = GetOwner();
	
	if (ownerActor == nullptr)
	{
		//PW_Utilities::Log("OWNER ACTOR NOT FOUND!"); return;
	}

	_ownerCharacter = Cast<APW_Character>(ownerActor);
	
	if (_ownerCharacter == nullptr)
	{
		//PW_Utilities::Log("OWNER CHARACTER NOT FOUND!");
	}
}

bool UPW_CharacterMovementComponent::ShouldReduceStamina()
{
	const UCharacterMovementComponent* characterMovement = _ownerCharacter->GetCharacterMovement();
	const float horizontalVelocityLength = characterMovement->Velocity.Size2D();
	
	return _isSprinting && horizontalVelocityLength > UE_SMALL_NUMBER;
}

bool UPW_CharacterMovementComponent::ShouldIncreaseStamina()
{
	const UCharacterMovementComponent* characterMovement = _ownerCharacter->GetCharacterMovement();
	const float horizontalVelocityLength = characterMovement->Velocity.Size2D();
	
	return !_isSprinting || horizontalVelocityLength < UE_SMALL_NUMBER;
}