// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "PROJECT_WEST/GameModes/PW_GameMode.h"
#include "PROJECT_WEST/GameModes/PW_BountyGameMode.h"
#include "PROJECT_WEST/PlayerState/PW_PlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "PROJECT_WEST/PW_HealthComponent.h"
#include "PROJECT_WEST/PlayerController/PW_PlayerController.h"

APW_Character::APW_Character(): _itemHolder(nullptr),
								_cameraComponent(nullptr),
                                _healthComponent(nullptr),
                                _overheadWidget(nullptr),
                                _springArmComponent(nullptr),
                                _playerController(nullptr),
                                _bountyGameMode(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
}

void APW_Character::BeginPlay()
{
	Super::BeginPlay();
}

void APW_Character::OnDeath(AActor* OwnerActor,AActor* DamageCauser, AController* DamageCauserController)
{
	_bountyGameMode = _bountyGameMode == nullptr ? GetWorld()->GetAuthGameMode<APW_BountyGameMode>() : _bountyGameMode;
	if (_bountyGameMode)
	{
		_playerController = _playerController == nullptr ? Cast<APW_PlayerController>(GetController()) : _playerController;
		_bountyGameMode->PlayerEliminated( this, _playerController, DamageCauserController );
	}
}

void APW_Character::OnHealthChanged()
{
	
}

void APW_Character::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (HasAuthority())
	{
		_healthComponent = FindComponentByClass<UPW_HealthComponent>();
		if (_healthComponent)
		{
			_healthComponent->OnHealthChangedGlobal.AddDynamic(this, &APW_Character::OnHealthChanged);
			_healthComponent->OnDeathGlobal.AddDynamic(this, &APW_Character::OnDeath);
		}
	}
}

void APW_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APW_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APW_Character, _playerName);
}

void APW_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APW_Character::JumpButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APW_Character::CrouchButtonPressed);
	PlayerInputComponent->BindAction("PrimaryUse", IE_Pressed, this, &APW_Character::UseButtonPressed);
	PlayerInputComponent->BindAction("PrimaryUse", IE_Released, this, &APW_Character::UseButtonReleased);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APW_Character::SprintButtonPressed);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APW_Character::SprintButtonReleased);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APW_Character::PickUpButtonPressed);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &APW_Character::StartInteractButtonPressed);
	PlayerInputComponent->BindAction("Return", IE_Released, this, &APW_Character::EndInteractButtonPressed);
	PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &APW_Character::DropButtonPressed);
	PlayerInputComponent->BindAction("Cycle", IE_Pressed, this, &APW_Character::SwitchItemButtonPressed);
	PlayerInputComponent->BindAction("SecondaryUse", IE_Pressed, this, &APW_Character::SecondaryUseButtonPressed);
	PlayerInputComponent->BindAction("SecondaryUse", IE_Released, this, &APW_Character::SecondaryUseButtonReleased);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APW_Character::ReloadButtonPressed);
	PlayerInputComponent->BindAction("MouseWheelUp", IE_Pressed, this, &APW_Character::MouseWheelUpAxisPressed);
	PlayerInputComponent->BindAction("MouseWheelDown", IE_Pressed, this, &APW_Character::MouseWheelDownAxisPressed);
	PlayerInputComponent->BindAction("SlotOne", IE_Pressed, this, &APW_Character::SlotOnePressed);
	PlayerInputComponent->BindAction("SlotTwo", IE_Pressed, this, &APW_Character::SlotTwoPressed);
	PlayerInputComponent->BindAction("SlotThree", IE_Pressed, this, &APW_Character::SlotThreePressed);
	PlayerInputComponent->BindAction("SlotFour", IE_Pressed, this, &APW_Character::SlotFourPressed);
	PlayerInputComponent->BindAxis("MoveForward", this, &APW_Character::MoveForwardAxisPressed);
	PlayerInputComponent->BindAxis("MoveRight", this, &APW_Character::MoveRightAxisPressed);
	PlayerInputComponent->BindAxis("LookUp", this, &APW_Character::LookUpAxisPressed);
	PlayerInputComponent->BindAxis("LookRight", this, &APW_Character::LookRightAxisPressed);
}

void APW_Character::ReloadButtonPressed()
{
	OnReloadButtonPressed.Broadcast();
}

void APW_Character::SecondaryUseButtonPressed()
{
	OnAimButtonPressed.Broadcast();
}

void APW_Character::SecondaryUseButtonReleased()
{
	OnAimButtonReleased.Broadcast();
}

void APW_Character::StartInteractButtonPressed()
{
	OnStartInteractButtonPressed.Broadcast();
}

void APW_Character::EndInteractButtonPressed()
{
	OnEndInteractButtonPressed.Broadcast();
}

void APW_Character::MouseWheelUpAxisPressed()
{
	OnMouseDownPressed.Broadcast();
}

void APW_Character::MouseWheelDownAxisPressed()
{
	OnMouseDownPressed.Broadcast();
}

void APW_Character::SlotOnePressed()
{
	OnSlotPressed.Broadcast(0);
}

void APW_Character::SlotTwoPressed()
{
	OnSlotPressed.Broadcast(1);
}

void APW_Character::SlotThreePressed()
{
	OnSlotPressed.Broadcast(2);
}

void APW_Character::SlotFourPressed()
{
	OnSlotPressed.Broadcast(3);
}

void APW_Character::JumpButtonPressed()
{
	OnJumpButtonPressed.Broadcast();
}

void APW_Character::SprintButtonPressed()
{
	OnSprintButtonPressed.Broadcast();

	if (_isPressedSprint)
		OnDashButtonPressed.Broadcast();
	
	_isPressedSprint = true;

	_sprintTimerDelegate.BindLambda([this]()
		{_isPressedSprint = false; });

	GetWorldTimerManager().SetTimer(_sprintTimerHandle, _sprintTimerDelegate,
		_sprintDoublePressTime, false);
}

void APW_Character::SprintButtonReleased()
{
	OnSprintButtonReleased.Broadcast();
}

void APW_Character::ToggleMovement(bool canMove)
{
	
}

void APW_Character::MoveForwardAxisPressed(float value)
{
	OnMoveForwardAxisPressed.Broadcast(value);
}

void APW_Character::MoveRightAxisPressed(float value)
{
	OnMoveRightAxisPressed.Broadcast(value);
}

void APW_Character::LookRightAxisPressed(float value)
{
	AddControllerYawInput(value);
	OnCameraRotationChange.Broadcast();
}

void APW_Character::LookUpAxisPressed(float value)
{
	AddControllerPitchInput(value);
	OnCameraRotationChange.Broadcast();
}

void APW_Character::UseButtonPressed()
{
	OnShootButtonPressed.Broadcast();
}

void APW_Character::UseButtonReleased()
{
	OnShootButtonReleased.Broadcast();
}

void APW_Character::CrouchButtonPressed()
{
	Super::Crouch();
}

void APW_Character::ServerLeaveGame_Implementation()
{
	/*APW_BountyGameMode * gameMode = Cast<APW_BountyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	APW_PlayerState* playerState = GetPlayerState<APW_PlayerState>();
	if (gameMode && playerState)
	{
		gameMode->PlayerLeftGame(playerState);
	}

	_itemHandlerComponent->DropAllItems();*/
}

void APW_Character::Elim(bool leftGame)
{
	MultiCastElim(leftGame);
}

void APW_Character::MultiCastElim_Implementation(bool leftGame)
{
	_LeftGame = leftGame;

	APW_GameMode* gameMode = Cast<APW_GameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	
	if (_LeftGame && IsLocallyControlled())
	{
		OnLeftGameDelegate.Broadcast();
	}
}

void APW_Character::PickUpButtonPressed()
{
	OnPickUpButtonPressed.Broadcast();
}

void APW_Character::SwitchItemButtonPressed()
{
	OnCycleItemButtonPressed.Broadcast();
}

void APW_Character::DropButtonPressed()
{
	OnDropButtonPressed.Broadcast();
}

bool APW_Character::IsAlive() const
{
	return  _healthComponent ? _healthComponent->IsAlive() : false;
}
