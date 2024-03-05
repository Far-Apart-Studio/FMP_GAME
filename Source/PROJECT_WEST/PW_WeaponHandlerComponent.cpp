// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_WeaponHandlerComponent.h"
#include "PWMath.h"
#include "PW_Character.h"
#include "PW_Utilities.h"
#include "PW_Weapon.h"
#include "Camera/CameraComponent.h"
#include "Engine/DamageEvents.h"
#include "PW_ItemHandlerComponent.h"

UPW_WeaponHandlerComponent::UPW_WeaponHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	_defaultWeaponData = nullptr;
	_ownerCharacter = nullptr;
	_defaultWeaponVisualData = nullptr;
	_itemHandlerComponent = nullptr;
}

void UPW_WeaponHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SetIsReplicated(true);
	GetOwnerCharacter();

	if(_ownerCharacter->IsLocallyControlled())
		AssignInputActions();
}

void UPW_WeaponHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (_ownerCharacter && _ownerCharacter->IsLocallyControlled())
		_lastFiredTime += DeltaTime;
}

APW_Weapon* UPW_WeaponHandlerComponent::TryGetCurrentWeapon() const
{
	APW_Weapon* currentWeapon = Cast<APW_Weapon>(_itemHandlerComponent->GetItemInHand());
	return currentWeapon;
}

void UPW_WeaponHandlerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UPW_WeaponHandlerComponent::BeginFireSequence()
{
	_isFiring = true;

	APW_Weapon* currentWeapon = TryGetCurrentWeapon();
	
	if (currentWeapon == nullptr)
		{PW_Utilities::Log("NO CURRENT WEAPON EQUIPPED!"); return; }

	UPW_WeaponData* weaponData = currentWeapon->GetWeaponData();

	if (weaponData == nullptr)
		{PW_Utilities::Log("NO WEAPON DATA FOUND!"); return; }
	
	CoreFireSequence(currentWeapon, weaponData);
}

void UPW_WeaponHandlerComponent::CoreFireSequence(APW_Weapon* currentWeapon, UPW_WeaponData* weaponData)
{
	if (!_isFiring)
		return;

	if (weaponData->GetWeaponFireType(_fireMode) == EFireType::Automatic)
		QueueAutomaticFire(currentWeapon, weaponData);

	if (currentWeapon->IsAmmoEmpty())
		{ ReloadWeapon(); return; }

	const bool canFire = CalculateFireStatus();
	
	if (canFire)
		CastBulletRays(weaponData, currentWeapon);
}

void UPW_WeaponHandlerComponent::CompleteFireSequence()
{
	_isFiring = false;

	const APW_Weapon* currentWeapon = TryGetCurrentWeapon();

	if (currentWeapon == nullptr)
		{ PW_Utilities::Log("NO CURRENT WEAPON EQUIPPED!"); return; }
	
	currentWeapon->OnWeaponStopFire.Broadcast();
}

void UPW_WeaponHandlerComponent::CastBulletRays(const UPW_WeaponData* weaponData, const APW_Weapon* currentWeapon)
{
	if(_ownerCharacter == nullptr)
		{ PW_Utilities::Log("COULD NOT FIND CHARACTER OWNER"); return; }
	
	UCameraComponent* cameraComponent = _ownerCharacter->GetCameraComponent();

	if (cameraComponent == nullptr)
		{ PW_Utilities::Log("NO CAMERA COMPONENT FOUND!"); return; }

	int projectileCount = weaponData->GetProjectileCount(_fireMode);
	const float defaultProjectileDelay = weaponData->GetProjectileDelay(_fireMode);
	float currentDelay = -defaultProjectileDelay;

	const int availableAmmo = currentWeapon->GetCurrentAmmo();
	if (availableAmmo < projectileCount)
		projectileCount = availableAmmo;
	
	for (int i = 0; i < projectileCount; i++)
	{
		currentDelay += defaultProjectileDelay;
		currentDelay += 0.000001f;
		
		FTimerDelegate fireRateDelegate;
		FTimerHandle _fireRateTimerHandle;
		
		fireRateDelegate.BindLambda([this, cameraComponent, weaponData, currentWeapon]()
			{ CastBulletRay(cameraComponent, weaponData, currentWeapon); });

		GetWorld()->GetTimerManager().SetTimer(_fireRateTimerHandle, fireRateDelegate,
			currentDelay, false);
	}
}

void UPW_WeaponHandlerComponent::CastBulletRay(UCameraComponent* cameraComponent, const UPW_WeaponData* weaponData, const APW_Weapon* currentWeapon)
{
	currentWeapon->OnWeaponFireDelegate.Broadcast();
	
	FVector rayDirection = cameraComponent->GetForwardVector();
	FVector rayStart = cameraComponent->GetComponentLocation();
	SimulateBulletSpread(rayDirection, weaponData);
	FVector rayDestination = rayStart + (rayDirection * 10000.0f);
	
	FCollisionQueryParams collisionQueryParams;
	collisionQueryParams.AddIgnoredActor(_ownerCharacter);
	FHitResult hitResult;

	bool isActorHit = CastRay(rayStart, rayDestination, collisionQueryParams, hitResult);
	currentWeapon->OnWeaponHit.Broadcast(hitResult, rayStart, rayDestination);
	
	if (isActorHit)
	{
		DrawDebugLine(GetWorld(), rayStart, rayDestination, FColor::Green,
			false, 2.0f, 0, 2.0f);
	}
	else
	{
		DrawDebugLine(GetWorld(), rayStart, rayDestination, FColor::Red,
			false, 2.0f, 0, 2.0f);
	}
	
	ApplyDamage(hitResult);
	if (currentWeapon->IsAmmoEmpty())
		ReloadWeapon(); 
}

void UPW_WeaponHandlerComponent::SimulateBulletSpread(FVector& rayDirection, const UPW_WeaponData* weaponData)
{
	const float weaponSpread = weaponData->GetWeaponAccuracy(_fireMode);
	const FVector spreadVector = FMath::VRandCone(rayDirection, weaponSpread);
	rayDirection = spreadVector;
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

void UPW_WeaponHandlerComponent::QueueAutomaticFire(APW_Weapon* currentWeapon, UPW_WeaponData* weaponData)
{
	const float fireRate = currentWeapon->GetWeaponData()->GetWeaponFireRate(_fireMode);
	FTimerDelegate automaticFireDelegate;
	automaticFireDelegate.BindLambda([this, currentWeapon, weaponData]()
		{ CoreFireSequence(currentWeapon, weaponData); });
	GetWorld()->GetTimerManager().SetTimer(_fireTimerHandle, automaticFireDelegate, fireRate, false);
}

void UPW_WeaponHandlerComponent::ReloadWeapon()
{
	GetOwner()->HasAuthority() ? LocalReloadWeapon() : ServerReloadWeapon();
}

void UPW_WeaponHandlerComponent::ServerReloadWeapon_Implementation()
{
	if (!GetOwner()->HasAuthority())
		return;
	
	LocalReloadWeapon();
}

void UPW_WeaponHandlerComponent::LocalReloadWeapon()
{
	APW_Weapon* currentWeapon = TryGetCurrentWeapon();
	
	if (currentWeapon == nullptr)
		{ PW_Utilities::Log("NO CURRENT WEAPON EQUIPPED!"); return; }
	
	if (currentWeapon->IsReloading())
		return;

	currentWeapon->OnWeaponBeginReload.Broadcast();
	currentWeapon->SetReloading(true);

	const UPW_WeaponData* weaponData = currentWeapon->GetWeaponData();

	if (weaponData == nullptr)
		{ PW_Utilities::Log("NO WEAPON DATA FOUND!"); return; }
	
	const float reloadTime = weaponData->GetWeaponReloadTime();
	
	_ownerCharacter->GetWorldTimerManager().SetTimer (_reloadTimerHandle, this,
		&UPW_WeaponHandlerComponent::OnReloadWeaponComplete, reloadTime, false);
}

void UPW_WeaponHandlerComponent::OnReloadWeaponComplete()
{
	APW_Weapon* currentWeapon = TryGetCurrentWeapon();
	
	if (currentWeapon == nullptr)
		{ PW_Utilities::Log("NO CURRENT WEAPON EQUIPPED!"); return; }
	
	currentWeapon->SetReloading(false);
	currentWeapon->TransferReserveAmmo();
}

void UPW_WeaponHandlerComponent::ApplyDamage(const FHitResult& hitResult)
{
	if (GetOwner()->HasAuthority())
		LocalApplyDamage(hitResult);
	else
		ServerApplyDamage(hitResult);
}

void UPW_WeaponHandlerComponent::ServerApplyDamage_Implementation(const FHitResult& hitResult)
{
	if (!GetOwner()->HasAuthority()) return;
	
	LocalApplyDamage(hitResult);
}

void UPW_WeaponHandlerComponent::LocalApplyDamage(const FHitResult& hitResult)
{
	APW_Weapon* currentWeapon = TryGetCurrentWeapon();
	if (currentWeapon == nullptr)
		{ PW_Utilities::Log("NO CURRENT WEAPON EQUIPPED!"); return; }

	currentWeapon->SubtractCurrentAmmo(1);

	const UPW_WeaponData* weaponData = currentWeapon->GetWeaponData();
	if (weaponData == nullptr)
		{ PW_Utilities::Log("NO WEAPON DATA FOUND!"); return; }
	
	AActor* hitActor = hitResult.GetActor();

	if (hitActor == nullptr)
		{ PW_Utilities::Log("HIT ACTOR NOT FOUND!"); return; }

	const float calculatedDamage = CalculateDamage(hitResult, weaponData);
	hitActor->TakeDamage(calculatedDamage, FDamageEvent(),
	_ownerCharacter->GetController(), _ownerCharacter);
}

float UPW_WeaponHandlerComponent::CalculateDamage(const FHitResult& hitResult, const UPW_WeaponData* weaponData)
{
	const float shotDistance = hitResult.Location.Distance(hitResult.TraceStart, hitResult.ImpactPoint);
	const float maximumDistance = weaponData->GetWeaponMaximumDistance(_fireMode);
	const float normalisedDistance = PWMath::Clamp01(1.0f - (shotDistance / maximumDistance));
	const float weaponDamage = weaponData->GetWeaponDamage(_fireMode);

	const UCurveFloat* fallOffCurve = weaponData->GetWeaponFallOffCurve(_fireMode);

	const float normalisedFallOff = fallOffCurve == nullptr
		                                ? normalisedDistance
		                                : fallOffCurve->GetFloatValue(normalisedDistance);
	
	const float calculatedDamage = weaponDamage * normalisedFallOff;
	return calculatedDamage;
}

bool UPW_WeaponHandlerComponent::CalculateFireStatus()
{
	const UPW_WeaponData* weaponData = TryGetCurrentWeapon()->GetWeaponData();

	if (weaponData == nullptr)
		{ PW_Utilities::Log("NO WEAPON DATA FOUND!"); return false; }
	
	if (_lastFiredTime < weaponData->GetWeaponFireRate(_fireMode))
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
		(this, &UPW_WeaponHandlerComponent::BeginFireSequence);

	_ownerCharacter->OnShootButtonReleased.AddDynamic
		(this, &UPW_WeaponHandlerComponent::CompleteFireSequence);

	_ownerCharacter->OnReloadButtonPressed.AddDynamic
		(this, &UPW_WeaponHandlerComponent::ReloadWeapon);
}


