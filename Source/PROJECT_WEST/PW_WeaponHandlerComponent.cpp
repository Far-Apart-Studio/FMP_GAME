// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_WeaponHandlerComponent.h"
#include "PWMath.h"
#include "PW_Character.h"
#include "PW_Utilities.h"
#include "PW_Weapon.h"
#include "Camera/CameraComponent.h"
#include "Engine/DamageEvents.h"
#include "Particles/ParticleSystemComponent.h"
#include "PW_ItemHandlerComponent.h"
#include "DebugMacros.h"
#include "Net/UnrealNetwork.h"

UPW_WeaponHandlerComponent::UPW_WeaponHandlerComponent()
{
	_defaultWeaponData = nullptr;
	//_currentWeapon = nullptr;
	_ownerCharacter = nullptr;
	_defaultWeaponVisualData = nullptr;
	
	PrimaryComponentTick.bCanEverTick = true;
}

void UPW_WeaponHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SetIsReplicated( true );
	
	if (GetOwner()->HasAuthority())
	{
		//DEBUG_STRING("HasAuthority : UPW_WeaponHandlerComponent::BeginPlay()");
	}
	else
	{
		//DEBUG_STRING("HasNoAuthority : UPW_WeaponHandlerComponent::BeginPlay()");
	}

	GetOwnerCharacter();

	if(_ownerCharacter->IsLocallyControlled())
	{
		AssignInputActions();
	}
	
	//AttachDefaultWeapon();
}

APW_Weapon* UPW_WeaponHandlerComponent::TryGetCurrentWeapon()
{
	APW_Weapon* weapon = Cast < APW_Weapon >(_itemHandlerComponent->GetItemInHand());
	return weapon;
}

void UPW_WeaponHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (_ownerCharacter && _ownerCharacter->IsLocallyControlled())
	{
		_lastFiredTime += DeltaTime;
	}
}

void UPW_WeaponHandlerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UPW_WeaponHandlerComponent::CastBulletRay()
{
	if(!_ownerCharacter)
	{
		DEBUG_STRING( "Owner Character Not Found!")
		return;
	}
	
	UCameraComponent* cameraComponent = _ownerCharacter->GetCameraComponent();
	
	FVector rayDirection = cameraComponent->GetForwardVector();
	FVector rayStart = cameraComponent->GetComponentLocation();
	FVector rayDestination = rayStart + (rayDirection * 10000.0f);
	
	FCollisionQueryParams collisionQueryParams;
	collisionQueryParams.AddIgnoredActor(_ownerCharacter);
	FHitResult hitResult;

	bool isActorHit = CastRay(rayStart, rayDestination, collisionQueryParams, hitResult);
	
	if (isActorHit)
	{
		DrawDebugLine(GetWorld(), rayStart, rayDestination, FColor::Green,
			false, 2.0f, 0, 2.0f);
		//PW_Utilities::Log("Hit Actor: ", hitResult.GetActor()->GetName());
		//DEBUG_STRING("Hit Actor: " + hitResult.GetActor()->GetName());
	}
	else
	{
		DrawDebugLine(GetWorld(), rayStart, rayDestination, FColor::Red,
			false, 2.0f, 0, 2.0f);
		//PW_Utilities::Log("No Actor Hit!");
		//DEBUG_STRING("Hit Actor:");
	}
	
	DoApplyDamage(hitResult);
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
	if (TryGetCurrentWeapon() == nullptr)
	{
		PW_Utilities::Log("NO CURRENT WEAPON EQUIPPED!");
		return;
	}

	if (TryGetCurrentWeapon()->IsAmmoEmpty())
	{
		DoReloadWeapon(); return;
	}

	const bool canFire = CalculateFireStatus();
	
	if (canFire)
	{
		FireWeaponVisual();
		CastBulletRay();
	}
}

void UPW_WeaponHandlerComponent::DoReloadWeapon()
{
	if (GetOwner()->HasAuthority())
		ReloadWeapon();
	else
		ServerReloadWeapon();
}

void UPW_WeaponHandlerComponent::ServerReloadWeapon_Implementation()
{
	if (!GetOwner()->HasAuthority())
		return;
	
	ReloadWeapon();
}

void UPW_WeaponHandlerComponent::ReloadWeapon()
{
	if (TryGetCurrentWeapon() == nullptr)
	{ PW_Utilities::Log("NO CURRENT WEAPON EQUIPPED!"); return; }
	
	if (TryGetCurrentWeapon()->IsReloading())
		return;
	
	TryGetCurrentWeapon()->SetReloading(true);

	const UPW_WeaponData* weaponData = TryGetCurrentWeapon()->GetWeaponData();

	if (weaponData == nullptr)
	{ PW_Utilities::Log("NO WEAPON DATA FOUND!"); return; }
	
	const float reloadTime = weaponData->GetWeaponReloadTime();
	
	_ownerCharacter->GetWorldTimerManager().SetTimer (_reloadTimerHandle, this, &UPW_WeaponHandlerComponent::OnReloadWeaponComplete, reloadTime, false);
}

void UPW_WeaponHandlerComponent::OnReloadWeaponComplete()
{
	if (TryGetCurrentWeapon() == nullptr) return;
	TryGetCurrentWeapon()->SetReloading(false);
	TryGetCurrentWeapon()->TransferReserveAmmo();
}

void UPW_WeaponHandlerComponent::DoApplyDamage(const FHitResult& hitResult)
{
	if (GetOwner()->HasAuthority())
		ApplyDamage(hitResult);
	else
		ServerApplyDamage(hitResult);
}

void UPW_WeaponHandlerComponent::ServerApplyDamage_Implementation(const FHitResult& hitResult)
{
	if (!GetOwner()->HasAuthority())
		return;
	
	ApplyDamage(hitResult);
}

void UPW_WeaponHandlerComponent::ApplyDamage(const FHitResult& hitResult)
{
	FireWeaponVisual();
	
	TryGetCurrentWeapon()->SubtractCurrentAmmo(1);
	
	AActor* hitActor = hitResult.GetActor();

	if (hitActor == nullptr)
	{ PW_Utilities::Log("HIT ACTOR NOT FOUND!"); return; }

	const float calculatedDamage = CalculateDamage(hitResult);
	hitActor->TakeDamage(calculatedDamage, FDamageEvent(),
	_ownerCharacter->GetController(), _ownerCharacter);
}

float UPW_WeaponHandlerComponent::CalculateDamage(const FHitResult& hitResult)
{
	const float shotDistance = hitResult.Location.Distance(hitResult.TraceStart, hitResult.ImpactPoint);

	PW_Utilities::Log("Shot Distance: ", shotDistance);
	
	float normalisedDamage = 1.0f - (shotDistance / _maximumWeaponFallOffRange);
	normalisedDamage = PWMath::Clamp01(normalisedDamage);

	const UPW_WeaponData* weaponData = TryGetCurrentWeapon()->GetWeaponData();
	const float weaponDamage = weaponData->GetHipWeaponDamage();

	PW_Utilities::Log("Normalised Damage: ", normalisedDamage);
	
	return  weaponDamage * normalisedDamage;
}

bool UPW_WeaponHandlerComponent::CalculateFireStatus()
{
	const UPW_WeaponData* weaponData = TryGetCurrentWeapon()->GetWeaponData();

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
	_itemHandlerComponent = Cast<UPW_ItemHandlerComponent>(_ownerCharacter->GetComponentByClass(UPW_ItemHandlerComponent::StaticClass()));

	if (_ownerCharacter == nullptr)
		{ PW_Utilities::Log("OWNER CHARACTER NOT FOUND!"); }
}

void UPW_WeaponHandlerComponent::AssignInputActions()
{
	_ownerCharacter->OnShootButtonPressed.AddDynamic
		(this, &UPW_WeaponHandlerComponent::FireWeapon);

	_ownerCharacter->OnReloadButtonPressed.AddDynamic
		(this, &UPW_WeaponHandlerComponent::DoReloadWeapon);
}

void UPW_WeaponHandlerComponent::FireWeaponVisual()
{
	TryGetCurrentWeapon()->GetMuzzleEffect()->ActivateSystem();
}

void UPW_WeaponHandlerComponent::ReloadWeaponVisual()
{
	
}

