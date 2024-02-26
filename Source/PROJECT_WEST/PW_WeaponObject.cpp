// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_WeaponObject.h"

#include "PWMath.h"
#include "PW_Character.h"
#include "PW_Utilities.h"
#include "PW_WeaponData.h"
#include "Camera/CameraComponent.h"
#include "Engine/DamageEvents.h"
#include "Net/UnrealNetwork.h"

APW_WeaponObject::APW_WeaponObject()
{
	_weaponData = nullptr;
	_weaponVisualData = nullptr;
	
	UStaticMeshComponent* itemMesh = GetItemMesh();

	_weaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	_weaponMesh->SetupAttachment(itemMesh);
	
	_muzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	_muzzleLocation->SetupAttachment(itemMesh);
}

void APW_WeaponObject::ApplyBindingActions(APW_Character* characterOwner)
{
	characterOwner->OnShootButtonPressed.AddDynamic(this, &APW_WeaponObject::BeginFireSequence);
	characterOwner->OnShootButtonReleased.AddDynamic(this, &APW_WeaponObject::CompleteFireSequence);
	characterOwner->OnReloadButtonPressed.AddDynamic(this, &APW_WeaponObject::ReloadWeapon);
}

void APW_WeaponObject::RemoveBindingActions(APW_Character* characterOwner)
{
	characterOwner->OnShootButtonPressed.RemoveDynamic(this, &APW_WeaponObject::BeginFireSequence);
	characterOwner->OnShootButtonReleased.RemoveDynamic(this, &APW_WeaponObject::CompleteFireSequence);
	characterOwner->OnReloadButtonPressed.RemoveDynamic(this, &APW_WeaponObject::ReloadWeapon);
}

void APW_WeaponObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APW_WeaponObject, _weaponRuntimeData);
}

void APW_WeaponObject::BeginFireSequence()
{
	_weaponRuntimeData.IsFiring = true;

	if (_weaponData == nullptr)
		{PW_Utilities::Log("NO WEAPON DATA FOUND!"); return; }
	
	CoreFireSequence();
}

void APW_WeaponObject::CoreFireSequence()
{
	if (!_weaponRuntimeData.IsFiring)
		return;

	if (_weaponData->GetHipWeaponFireType() == EFireType::Automatic)
		QueueAutomaticFire();
	
	if (IsAmmoEmpty())
		{ ReloadWeapon(); return; }
	
	if (CalculateFireStatus())
		CastBulletRays();
}

void APW_WeaponObject::CompleteFireSequence()
{
	_weaponRuntimeData.IsFiring = false;
}

void APW_WeaponObject::CastBulletRays()
{
	const AActor* owner = GetOwner();
	const APW_Character* ownerCharacter = Cast<APW_Character>(owner);
	
	if(ownerCharacter == nullptr)
		{ PW_Utilities::Log("COULD NOT FIND CHARACTER OWNER"); return; }
	
	UCameraComponent* cameraComponent = ownerCharacter->GetCameraComponent();

	if (cameraComponent == nullptr)
		{ PW_Utilities::Log("NO CAMERA COMPONENT FOUND!"); return; }

	OnWeaponFireDelegate.Broadcast();
	int projectileCount = _weaponData->GetHipProjectileCount();
	const float defaultProjectileDelay = _weaponData->GetHipProjectileDelay();
	float currentDelay = -defaultProjectileDelay;

	const int availableAmmo = _weaponRuntimeData.CurrentAmmo;
	if (availableAmmo < projectileCount)
		projectileCount = availableAmmo;
	
	for (int i = 0; i < projectileCount; i++)
	{
		currentDelay += defaultProjectileDelay;
		currentDelay += 0.000001f;
		
		FTimerDelegate fireRateDelegate;
		FTimerHandle _fireRateTimerHandle;
		
		fireRateDelegate.BindLambda([this, cameraComponent]()
			{ CastBulletRay(cameraComponent); });

		GetWorld()->GetTimerManager().SetTimer(_fireRateTimerHandle, fireRateDelegate,
			currentDelay, false);
	}
}

void APW_WeaponObject::CastBulletRay(UCameraComponent* cameraComponent)
{
	const AActor* owner = GetOwner();
	const APW_Character* ownerCharacter = Cast<APW_Character>(owner);
	
	if(ownerCharacter == nullptr)
		{ PW_Utilities::Log("COULD NOT FIND CHARACTER OWNER"); return; }
	
	FVector rayDirection = cameraComponent->GetForwardVector();
	FVector rayStart = cameraComponent->GetComponentLocation();
	SimulateBulletSpread(rayDirection);
	FVector rayDestination = rayStart + (rayDirection * 10000.0f);
	
	FCollisionQueryParams collisionQueryParams;
	collisionQueryParams.AddIgnoredActor(ownerCharacter);
	FHitResult hitResult;

	bool isActorHit = CastRay(rayStart, rayDestination, collisionQueryParams, hitResult);
	
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
	if (IsAmmoEmpty())
		ReloadWeapon(); 
}

void APW_WeaponObject::SimulateBulletSpread(FVector& rayDirection)
{
	const float weaponSpread = _weaponData->GetHipWeaponAccuracy();
	const FVector spreadVector = FMath::VRandCone(rayDirection, weaponSpread);
	rayDirection = spreadVector;
}

bool APW_WeaponObject::CastRay(const FVector& rayStart, const FVector& rayDestination,
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

void APW_WeaponObject::QueueAutomaticFire()
{
	const float fireRate = _weaponData->GetHipWeaponFireRate();
	FTimerDelegate automaticFireDelegate;
	automaticFireDelegate.BindLambda([this](){ CoreFireSequence(); });
	GetWorld()->GetTimerManager().SetTimer(_fireTimerHandle, automaticFireDelegate, fireRate, false);
}

void APW_WeaponObject::ReloadWeapon()
{
	GetOwner()->HasAuthority() ? LocalReloadWeapon() : ServerReloadWeapon();
}

void APW_WeaponObject::TransferReserveAmmo()
{
	if (_weaponRuntimeData.CurrentReserveAmmo <= 0)
		return;

	const int ammoRequired = _weaponData->GetWeaponMagazineCapacity() - _weaponRuntimeData.CurrentAmmo;
	const int ammoToTransfer = FMath::Min(ammoRequired, _weaponRuntimeData.CurrentReserveAmmo);
	
	_weaponRuntimeData.CurrentAmmo += ammoToTransfer;
	_weaponRuntimeData.CurrentReserveAmmo -= ammoToTransfer;
}

void APW_WeaponObject::ServerReloadWeapon_Implementation()
{
	if (!GetOwner()->HasAuthority())
		return;
	
	LocalReloadWeapon();
}

void APW_WeaponObject::LocalReloadWeapon()
{
	if (_weaponRuntimeData.IsReloading)
		return;

	OnWeaponBeginReload.Broadcast();
	_weaponRuntimeData.IsReloading = true;

	if (_weaponData == nullptr)
		{ PW_Utilities::Log("NO WEAPON DATA FOUND!"); return; }
	
	const float reloadTime = _weaponData->GetWeaponReloadTime();
	
	GetWorldTimerManager().SetTimer (_reloadTimerHandle, this,
		&APW_WeaponObject::OnReloadWeaponComplete, reloadTime, false);
}

void APW_WeaponObject::OnReloadWeaponComplete()
{
	_weaponRuntimeData.IsReloading = false;
	TransferReserveAmmo();
}

void APW_WeaponObject::ApplyDamage(const FHitResult& hitResult)
{
	if (GetOwner()->HasAuthority())
		LocalApplyDamage(hitResult);
	else
		ServerApplyDamage(hitResult);
}

void APW_WeaponObject::ServerApplyDamage_Implementation(const FHitResult& hitResult)
{
	if (!GetOwner()->HasAuthority()) return;
	
	LocalApplyDamage(hitResult);
}

void APW_WeaponObject::LocalApplyDamage(const FHitResult& hitResult)
{
	AActor* owner = GetOwner();
	APW_Character* ownerCharacter = Cast<APW_Character>(owner);
	
	if(ownerCharacter == nullptr)
	{ PW_Utilities::Log("COULD NOT FIND CHARACTER OWNER"); return; }
	
	_weaponRuntimeData.CurrentAmmo--;
	
	if (_weaponData == nullptr)
		{ PW_Utilities::Log("NO WEAPON DATA FOUND!"); return; }
	
	AActor* hitActor = hitResult.GetActor();

	if (hitActor == nullptr)
		{ PW_Utilities::Log("HIT ACTOR NOT FOUND!"); return; }

	const float calculatedDamage = CalculateDamage(hitResult);
	hitActor->TakeDamage(calculatedDamage, FDamageEvent(),
		ownerCharacter->GetController(), ownerCharacter);
}

float APW_WeaponObject::CalculateDamage(const FHitResult& hitResult)
{
	const float shotDistance = hitResult.Location.Distance(hitResult.TraceStart, hitResult.ImpactPoint);
	const float maximumDistance = _weaponData->GetHipWeaponMaximumDistance();
	const float normalisedDistance = PWMath::Clamp01(1.0f - (shotDistance / maximumDistance));
	const float weaponDamage = _weaponData->GetHipWeaponDamage();

	const UCurveFloat* fallOffCurve = _weaponData->GetHipWeaponFallOffCurve();

	const float normalisedFallOff = fallOffCurve == nullptr
		                                ? normalisedDistance
		                                : fallOffCurve->GetFloatValue(normalisedDistance);
	
	const float calculatedDamage = weaponDamage * normalisedFallOff;
	return calculatedDamage;
}

bool APW_WeaponObject::CalculateFireStatus()
{
	if (_weaponData == nullptr)
		{ PW_Utilities::Log("NO WEAPON DATA FOUND!"); return false; }
	
	if (_weaponRuntimeData.LastFiredTime < _weaponData->GetHipWeaponFireRate())
		return false;

	_weaponRuntimeData.LastFiredTime = 0.0f;
	return true;
}

