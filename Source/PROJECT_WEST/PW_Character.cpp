// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PROJECT_WEST/Items/PW_Item.h"
#include "Components/WidgetComponent.h"
#include "PROJECT_WEST/GameModes/PW_GameMode.h"
#include "PROJECT_WEST/GameModes/PW_BountyGameMode.h"
#include "PROJECT_WEST/PlayerState/PW_PlayerState.h"


APW_Character::APW_Character()
{
	PrimaryActorTick.bCanEverTick = true;
	
	_cameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	_cameraComponent->SetupAttachment(RootComponent);
	_cameraComponent->bUsePawnControlRotation = true;

	_objectHolder = CreateDefaultSubobject<USceneComponent>(TEXT("ObjectHolder"));
	_objectHolder->SetupAttachment(_cameraComponent);

	_overheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	_overheadWidget->SetupAttachment(RootComponent);

	_itemHolder = CreateDefaultSubobject<USceneComponent>(TEXT("ItemHolder"));
	_itemHolder->SetupAttachment(_cameraComponent);
}

void APW_Character::BeginPlay()
{
	Super::BeginPlay();
}

void APW_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APW_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);	
	
	DOREPLIFETIME(APW_Character, _overlappingItem);
	DOREPLIFETIME(APW_Character, _itemInHand);
}

void APW_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APW_Character::JumpButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APW_Character::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APW_Character::UseButtonPressed);
	PlayerInputComponent->BindAction("SprintToggle", IE_Pressed, this, &APW_Character::SprintButtonPressed);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &APW_Character::EquipButtonPressed);
	PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &APW_Character::DropButtonPressed);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &APW_Character::MoveForwardAxisPressed);
	PlayerInputComponent->BindAxis("MoveRight", this, &APW_Character::MoveRightAxisPressed);
	PlayerInputComponent->BindAxis("LookUp", this, &APW_Character::LookUpAxisPressed);
	PlayerInputComponent->BindAxis("LookRight", this, &APW_Character::LookRightAxisPressed);
}

void APW_Character::MoveForwardAxisPressed(float value)
{
	const FVector moveDirection = GetActorForwardVector();
	AddMovementInput(moveDirection, value);
}

void APW_Character::MoveRightAxisPressed(float value)
{
	const FVector moveDirection = GetActorRightVector();
	AddMovementInput(moveDirection, value);
}

void APW_Character::LookRightAxisPressed(float value)
{
	AddControllerYawInput(value);
}

void APW_Character::LookUpAxisPressed(float value)
{
	AddControllerPitchInput(value);
}

void APW_Character::SprintButtonPressed()
{
	_isSprinting = !_isSprinting;
	
	_isSprinting ?
		GetCharacterMovement()->MaxWalkSpeed *= _sprintMultiplier :
		GetCharacterMovement()->MaxWalkSpeed /= _sprintMultiplier;
}

void APW_Character::JumpButtonPressed()
{
	Super::Jump();
}

void APW_Character::UseButtonPressed()
{
	OnShootButtonPressed.Broadcast();
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
}

void APW_Character::Elim(bool leftGame)
{
	DropItem();
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

void APW_Character::SetOverlappingItem(APW_Item* Item)
{
	if (_overlappingItem)
	{
		_overlappingItem->ShowPickupWidget(false);
	}
	_overlappingItem = Item;
	if (IsLocallyControlled())
	{
		if (_overlappingItem)
		{
			_overlappingItem->ShowPickupWidget(true);
		}
	}
}

void APW_Character::EquipItem(APW_Item* Apw_Item)
{
	_itemInHand = Apw_Item;
	Apw_Item->SetItemState(EItemState::EIS_Equipped);
	Apw_Item->SetOwner(this);
	Apw_Item->AttachToComponent(_itemHolder, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void APW_Character::DropItem()
{
	if(!_itemInHand) return;
	_itemInHand->SetItemState(EItemState::EIS_Dropped);
	_itemInHand->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	_itemInHand->SetOwner(nullptr);
	_itemInHand = nullptr;
}

void APW_Character::EquipButtonPressed()
{
	ServerEquipButtonPressed();
}

void APW_Character::DropButtonPressed()
{
	ServerDropButtonPressed();
}

void APW_Character::ServerEquipButtonPressed_Implementation()
{
	if (_overlappingItem)
	{
		if (_itemInHand)
		{
			DropButtonPressed();
		}
		
		EquipItem(_overlappingItem);
		_overlappingItem = nullptr;
	}
}

void APW_Character::ServerDropButtonPressed_Implementation()
{
	if (_itemInHand)
	{
		DropItem();
	}
}

void APW_Character::OnRep_WeaponChange(APW_Item* LastWeapon)
{
	if (LastWeapon)
	{
		LastWeapon->SetItemState(EItemState::EIS_Dropped);
		LastWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		LastWeapon->SetOwner(nullptr);
	}
	
	if (_itemInHand)
	{
		_itemInHand->SetItemState(EItemState::EIS_Equipped);
		_itemInHand->SetOwner(this);
		_itemInHand->AttachToComponent(_itemHolder, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
}

void APW_Character::OnRep_OverlappinItem(APW_Item* lastItem)
{
	if (_overlappingItem)
	{
		_overlappingItem->ShowPickupWidget(true);
	}
	
	if (lastItem)
	{
		lastItem->ShowPickupWidget(false);
	}
}





