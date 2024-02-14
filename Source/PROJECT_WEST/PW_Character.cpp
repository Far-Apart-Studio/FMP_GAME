// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "PROJECT_WEST/GameModes/PW_GameMode.h"
#include "PROJECT_WEST/GameModes/PW_BountyGameMode.h"
#include "PROJECT_WEST/PlayerState/PW_PlayerState.h"
#include "PW_ItemHandlerComponent.h"
#include "PW_WeaponHandlerComponent.h"
#include "PROJECT_WEST/PW_HealthComponent.h"
#include "PROJECT_WEST/PlayerController/PW_PlayerController.h"

APW_Character::APW_Character()
{
	PrimaryActorTick.bCanEverTick = true;
	
	_cameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	_cameraComponent->SetupAttachment(RootComponent);
	_cameraComponent->bUsePawnControlRotation = true;
	
	_overheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	_overheadWidget->SetupAttachment(RootComponent);

	_itemHolder = CreateDefaultSubobject<USceneComponent>(TEXT("ItemHolder"));
	_itemHolder->SetupAttachment(_cameraComponent);
}

void APW_Character::BeginPlay()
{
	Super::BeginPlay();
}

void APW_Character::OnDeath(AActor* DamageCauser, AController* DamageCauserController)
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

	UPW_WeaponHandlerComponent* weaponHandler = FindComponentByClass<UPW_WeaponHandlerComponent>();
	if (weaponHandler)
		weaponHandler->SetOwnerCharacter(this);

	_itemHandlerComponent = FindComponentByClass<UPW_ItemHandlerComponent>();
	if (_itemHandlerComponent)
		_itemHandlerComponent->SetOwnerCharacter(this);

	if (HasAuthority())
	{
		_healthComponent = FindComponentByClass<UPW_HealthComponent>();
		if (_healthComponent)
		{
			_healthComponent->OnHealthChanged.AddDynamic(this, &APW_Character::OnHealthChanged);
			_healthComponent->OnDeath.AddDynamic(this, &APW_Character::OnDeath);
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
}

void APW_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APW_Character::JumpButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APW_Character::CrouchButtonPressed);
	PlayerInputComponent->BindAction("PrimaryUse", IE_Pressed, this, &APW_Character::UseButtonPressed);
	PlayerInputComponent->BindAction("PrimaryUse", IE_Released, this, &APW_Character::UseButtonReleased);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APW_Character::SprintButtonPressed);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APW_Character::PickUpButtonPressed);
	PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &APW_Character::DropButtonPressed);
	PlayerInputComponent->BindAction("Switch", IE_Pressed, this, &APW_Character::SwitchItemButtonPressed);
	PlayerInputComponent->BindAxis("MoveForward", this, &APW_Character::MoveForwardAxisPressed);
	PlayerInputComponent->BindAxis("MoveRight", this, &APW_Character::MoveRightAxisPressed);
	PlayerInputComponent->BindAxis("LookUp", this, &APW_Character::LookUpAxisPressed);
	PlayerInputComponent->BindAxis("LookRight", this, &APW_Character::LookRightAxisPressed);
}

void APW_Character::JumpButtonPressed()
{
	OnJumpButtonPressed.Broadcast();
}

void APW_Character::SprintButtonPressed()
{
	OnSprintButtonPressed.Broadcast();
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
}

void APW_Character::LookUpAxisPressed(float value)
{
	AddControllerPitchInput(value);
}

void APW_Character::UseButtonPressed()
{
	OnShootButtonPressed.Broadcast();
}

void APW_Character::UseButtonReleased()
{
	OnShootReleaseDelegate.Broadcast();
}

void APW_Character::CrouchButtonPressed()
{
	Super::Crouch();
}

void APW_Character::ServerLeaveGame_Implementation()
{
	APW_BountyGameMode * gameMode = Cast<APW_BountyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	APW_PlayerState* playerState = GetPlayerState<APW_PlayerState>();
	if (gameMode && playerState)
	{
		gameMode->PlayerLeftGame(playerState);
	}

	_itemHandlerComponent->DropAllItems();
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
	OnSwitchItemButtonPressed.Broadcast();
}

void APW_Character::DropButtonPressed()
{
	OnDropButtonPressed.Broadcast();
}

bool APW_Character::IsAlive() const
{
	return  _healthComponent ? _healthComponent->IsAlive() : false;
}
