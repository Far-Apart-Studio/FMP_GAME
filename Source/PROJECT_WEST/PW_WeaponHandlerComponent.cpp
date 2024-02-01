// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_WeaponHandlerComponent.h"
#include "PWMath.h"
#include "PW_Character.h"
#include "PW_Utilities.h"
#include "PW_Weapon.h"
#include "Camera/CameraComponent.h"
#include "Engine/DamageEvents.h"

UPW_WeaponHandlerComponent::UPW_WeaponHandlerComponent()
{
	_defaultWeaponData = nullptr;
	_currentWeapon = nullptr;
	_ownerCharacter = nullptr;
	
	PrimaryComponentTick.bCanEverTick = true;
}

void UPW_WeaponHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	GetOwnerCharacter();
	AssignInputActions();
	AttachDefaultWeapon();
}

void UPW_WeaponHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	_lastFiredTime += DeltaTime;
}

void UPW_WeaponHandlerComponent::CastBulletRay()
{
	UCameraComponent* cameraComponent = _ownerCharacter->GetCameraComponent();
	
	FVector rayDirection = cameraComponent->GetForwardVector();
	FVector rayStart = cameraComponent->GetComponentLocation();
	FVector rayDestination = rayStart + (rayDirection * 10000.0f);
	
	FCollisionQueryParams collisionQueryParams;
	collisionQueryParams.AddIgnoredActor(_ownerCharacter);
	FHitResult hitResult;

	bool isActorHit = CastRay(rayStart, rayDestination, collisionQueryParams, hitResult);
	
	if (isActorHit)
		DrawDebugLine(GetWorld(), rayStart, rayDestination, FColor::Green,
			false, 2.0f, 0, 2.0f);
	else
		DrawDebugLine(GetWorld(), rayStart, rayDestination, FColor::Red,
			false, 2.0f, 0, 2.0f);

	ApplyDamage(hitResult);
}

bool UPW_WeaponHandlerComponent::CastRay(const FVector& rayStart, const FVector& rayDestination,
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

void UPW_WeaponHandlerComponent::FireWeapon()
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

void UPW_WeaponHandlerComponent::ReloadWeapon()
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

	_ownerCharacter->GetWorldTimerManager().SetTimer
		(reloadTimerHandle, reloadTimerDelegate, reloadTime, false);
}

void UPW_WeaponHandlerComponent::AttachDefaultWeapon()
{
	USceneComponent* weaponHolder = _ownerCharacter->GetItemHolder();
	
	UWorld* currentWorld = GetWorld();
	FActorSpawnParameters spawnParameters;
	spawnParameters.Owner = _ownerCharacter;

	const FVector spawnLocation = weaponHolder->GetComponentLocation();
	const FRotator spawnRotation = weaponHolder->GetComponentRotation();

	_currentWeapon = currentWorld->SpawnActor<APW_Weapon>(spawnLocation, spawnRotation, spawnParameters);

	if (_currentWeapon == nullptr)
		{ PW_Utilities::Log("DEFAULT WEAPON NOT FOUND!"); return; }
	
	SetCurrentWeapon(_currentWeapon);
	_currentWeapon->AttachToComponent(weaponHolder, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	_currentWeapon->InitialiseWeapon(_defaultWeaponData);
}

void UPW_WeaponHandlerComponent::ApplyDamage(const FHitResult& hitResult) const
{
	AActor* hitActor = hitResult.GetActor();

	if (hitActor == nullptr)
		{ PW_Utilities::Log("HIT ACTOR NOT FOUND!"); return; }

	const float calculatedDamage = CalculateDamage(hitResult);
	hitActor->TakeDamage(calculatedDamage, FDamageEvent(),
		_ownerCharacter->GetController(), _ownerCharacter);
}

float UPW_WeaponHandlerComponent::CalculateDamage(const FHitResult& hitResult) const
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

bool UPW_WeaponHandlerComponent::CalculateFireStatus()
{
	const UPW_WeaponData* weaponData = _currentWeapon->GetWeaponData();

	if (weaponData == nullptr)
		{ PW_Utilities::Log("NO WEAPON DATA FOUND!"); return false; }
	
	if (_lastFiredTime < weaponData->GetHipWeaponFireRate())
		return false;

	_lastFiredTime = 0.0f;
	return true;
}

void UPW_WeaponHandlerComponent::GetOwnerCharacter()
{
	AActor* ownerActor = GetOwner();
	
	if (ownerActor == nullptr)
		{ PW_Utilities::Log("OWNER ACTOR NOT FOUND!"); return; }

	_ownerCharacter = Cast<APW_Character>(ownerActor);

	if (_ownerCharacter == nullptr)
		{ PW_Utilities::Log("OWNER CHARACTER NOT FOUND!"); }
}

void UPW_WeaponHandlerComponent::AssignInputActions()
{
	_ownerCharacter->OnShootButtonPressed.AddDynamic
		(this, &UPW_WeaponHandlerComponent::FireWeapon);

	_ownerCharacter->OnReloadButtonPressed.AddDynamic
		(this, &UPW_WeaponHandlerComponent::ReloadWeapon);
}

