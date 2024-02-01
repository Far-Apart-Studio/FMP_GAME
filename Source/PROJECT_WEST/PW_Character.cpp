// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_Character.h"

#include "PWMath.h"
#include "PW_Utilities.h"
#include "PW_Weapon.h"
#include "Camera/CameraComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "PROJECT_WEST/Items/PW_Item.h"
#include "Components/WidgetComponent.h"
#include "PROJECT_WEST/GameModes/PW_BountyGameMode.h"
#include "PROJECT_WEST/PlayerState/PW_PlayerState.h"


APW_Character::APW_Character()
{
	PrimaryActorTick.bCanEverTick = true;
	
	_cameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	_cameraComponent->SetupAttachment(RootComponent);
	_cameraComponent->bUsePawnControlRotation = true;

	_weaponHolder = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponHolder"));
	_weaponHolder->SetupAttachment(RootComponent);

	_overheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	_overheadWidget->SetupAttachment(RootComponent);

	_itemHolder = CreateDefaultSubobject<USceneComponent>(TEXT("ItemHolder"));
	_itemHolder->SetupAttachment(_cameraComponent);
}

void APW_Character::BeginPlay()
{
	Super::BeginPlay();
	AttachDefaultWeapon();
}

void APW_Character::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,AController* InstigatorController, AActor* DamageCauser)
{
	
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
			DropItem();
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

void APW_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// >>> ------------------ Weapon Handler Component ------------------ >>> //
	_lastFiredTime += DeltaTime;
	// <<< ------------------ Weapon Handler Component ------------------ <<< //
}

void APW_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);	
	
	DOREPLIFETIME(APW_Character, _overlappingItem);
}

// >>> ------------------ Weapon Handler Component ------------------ >>> //

void APW_Character::CastBulletRay()
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

	ApplyDamage(hitResult);
}

bool APW_Character::CastRay(const FVector& rayStart, const FVector& rayDestination,
	const FCollisionQueryParams& collisionQueryParams, FHitResult& hitResult) const
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

void APW_Character::FireWeapon()
{
	if (_currentWeapon == nullptr)
		{ PW_Utilities::Log("NO CURRENT WEAPON EQUIPPED!"); return; }

	if (_currentWeapon->IsAmmoEmpty())
		{ ReloadWeapon(); return; }

	const bool canFire = CalculateFireStatus();
	
	if (canFire)
	{
		CastBulletRay();
		_currentWeapon->SubtractCurrentAmmo(1);
	}
} 

void APW_Character::ReloadWeapon()
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
void APW_Character::AttachDefaultWeapon()
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

void APW_Character::ApplyDamage(const FHitResult& hitResult)
{
	AActor* hitActor = hitResult.GetActor();

	if (hitActor == nullptr)
		{ PW_Utilities::Log("HIT ACTOR NOT FOUND!"); return; }

	const float calculatedDamage = CalculateDamage(hitResult);
	hitActor->TakeDamage(calculatedDamage, FDamageEvent(),
		GetController(), this);
}

float APW_Character::CalculateDamage(const FHitResult& hitResult) const
{
	const float shotDistance = hitResult.Location.Distance(hitResult.TraceStart, hitResult.ImpactPoint);

	PW_Utilities::Log("Shot Distance: ", shotDistance);
	
	float normalisedDamage = 1.0f - (shotDistance / _maximumWeaponFallOffRange);
	normalisedDamage = PWMath::Clamp01(normalisedDamage);

	const UPW_WeaponData* weaponData = _currentWeapon->GetWeaponData();
	const float weaponDamage = weaponData->GetHipWeaponDamage();

	PW_Utilities::Log("Normalised Damage: ", normalisedDamage);
	
	return  weaponDamage * normalisedDamage;
}

bool APW_Character::CalculateFireStatus()
{
	const UPW_WeaponData* weaponData = _currentWeapon->GetWeaponData();

	if (weaponData == nullptr)
		{ PW_Utilities::Log("NO WEAPON DATA FOUND!"); return false; }
	
	if (_lastFiredTime < weaponData->GetHipWeaponFireRate())
		return false;

	_lastFiredTime = 0.0f;
	return true;
}

// <<< ------------------ Weapon Handler Component ------------------ <<< //


void APW_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APW_Character::Jump);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APW_Character::Crouch);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APW_Character::FireWeapon);
	PlayerInputComponent->BindAction("SprintToggle", IE_Pressed, this, &APW_Character::ToggleSprint);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &APW_Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APW_Character::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &APW_Character::LookRight);
	PlayerInputComponent->BindAxis("LookRight", this, &APW_Character::LookUp);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &APW_Character::EquipButtonPressed);
	PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &APW_Character::DropButtonPressed);
}

void APW_Character::MoveForward(float value)
{
	const FVector moveDirection = GetActorForwardVector();
	AddMovementInput(moveDirection, value);
}

void APW_Character::MoveRight(float value)
{
	const FVector moveDirection = GetActorRightVector();
	AddMovementInput(moveDirection, value);
}

void APW_Character::ToggleSprint()
{
	_isSprinting = !_isSprinting;
	
	_isSprinting ?
		GetCharacterMovement()->MaxWalkSpeed *= _sprintMultiplier :
		GetCharacterMovement()->MaxWalkSpeed /= _sprintMultiplier;
}

void APW_Character::LookRight(float value)
{
	AddControllerYawInput(value);
}

void APW_Character::LookUp(float value)
{
	AddControllerPitchInput(value);
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

// Move to item Handler Component
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

void APW_Character::Jump()
{
	Super::Jump();
}

void APW_Character::Crouch()
{
	Super::Crouch();
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





