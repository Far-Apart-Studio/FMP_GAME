// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_WeaponObject.h"

#include "FRecoilAction.h"
#include "PWMath.h"
#include "PW_Character.h"
#include "PW_CharacterMovementComponent.h"
#include "PW_DamageCauserComponent.h"
#include "PW_Utilities.h"
#include "PW_WeaponData.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

APW_WeaponObject::APW_WeaponObject()
{
	_weaponData = nullptr;
	_weaponVisualData = nullptr;
	
	UStaticMeshComponent* itemMesh = GetCollisionMesh();
	
	_muzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	_muzzleLocation->SetupAttachment(itemMesh);

	_damageCauserComponent = CreateDefaultSubobject<UPW_DamageCauserComponent>(TEXT("DamageCauserComponent"));
}

void APW_WeaponObject::BeginPlay()
{
	Super::BeginPlay();

	if (_weaponData == nullptr)
		{ PW_Utilities::Log("WEAPON DATA NOT FOUND!"); return; }

	_weaponRuntimeData.CurrentAmmo = _weaponData->GetWeaponMagazineCapacity();
	_weaponRuntimeData.CurrentReserveAmmo = _weaponData->GetWeaponReserveAmmunition();
}

void APW_WeaponObject::Tick(float DeltaSeconds)
{
	AActor* owner = GetOwner();
	const APW_Character* ownerCharacter = Cast<APW_Character>(owner);
	
	if (ownerCharacter == nullptr)
		return;

	if (ownerCharacter->IsLocallyControlled())
		_weaponRuntimeData.LastFiredTime += DeltaSeconds;
}

void APW_WeaponObject::ApplyObjectActions(APW_Character* characterOwner)
{
	Super::ApplyObjectActions(characterOwner);
	characterOwner->OnShootButtonPressed.AddDynamic(this, &APW_WeaponObject::BeginFireSequence);
	characterOwner->OnShootButtonReleased.AddDynamic(this, &APW_WeaponObject::CompleteFireSequence);
	characterOwner->OnReloadButtonPressed.AddDynamic(this, &APW_WeaponObject::QueueReloadWeapon);
	characterOwner->OnAimButtonPressed.AddDynamic(this, &APW_WeaponObject::FireModeAim);
	characterOwner->OnAimButtonReleased.AddDynamic(this, &APW_WeaponObject::FireModeHip);

	OnWeaponEquip.Broadcast(this);
}

void APW_WeaponObject::ClearObjectActions(APW_Character* characterOwner)
{
	Super::ClearObjectActions(characterOwner);
	characterOwner->OnShootButtonPressed.RemoveDynamic(this, &APW_WeaponObject::BeginFireSequence);
	characterOwner->OnShootButtonReleased.RemoveDynamic(this, &APW_WeaponObject::CompleteFireSequence);
	characterOwner->OnReloadButtonPressed.RemoveDynamic(this, &APW_WeaponObject::QueueReloadWeapon);
	characterOwner->OnAimButtonPressed.RemoveDynamic(this, &APW_WeaponObject::FireModeAim);
	characterOwner->OnAimButtonReleased.RemoveDynamic(this, &APW_WeaponObject::FireModeHip);

	OnWeaponUnEquip.Broadcast(this);
	CancelWeaponActions();
}

void APW_WeaponObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APW_WeaponObject, _weaponRuntimeData);
}

void APW_WeaponObject::CancelWeaponActions()
{
	_weaponRuntimeData.IsReloading = false;
	_weaponRuntimeData.IsFiring = false;

	FTimerManager& timerManager = GetWorld()->GetTimerManager();
	
	timerManager.ClearTimer(_reloadTimerHandle);
	timerManager.ClearTimer(_fireTimerHandle);

	FireModeHip();
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
	if (!GetIsActive())
		return;
	
	if (!_weaponRuntimeData.IsFiring)
		return;

	if (_weaponData->GetWeaponFireType(_weaponFireMode) == EFireType::Automatic)
		QueueAutomaticFire();
	
	
	if (IsAmmoEmpty())
		{ QueueReloadWeapon(); return; }
	
	if (CanFire())
	{
		CastBulletRays();
		const int currentAmmo = _weaponRuntimeData.CurrentAmmo - _weaponData->GetAmmoConsumption(_weaponFireMode);
		_weaponRuntimeData.CurrentAmmo = PWMath::ClampZero(currentAmmo);
		UpdateAmmo(_weaponRuntimeData.CurrentAmmo, _weaponRuntimeData.CurrentReserveAmmo);
	}
}

void APW_WeaponObject::CompleteFireSequence()
{
	_weaponRuntimeData.IsFiring = false;
	OnWeaponStopFire.Broadcast();
}

void APW_WeaponObject::CastBulletRays()
{
	const AActor* owner = GetOwner();
	const APW_Character* ownerCharacter = Cast<APW_Character>(owner);
	
	if (ownerCharacter == nullptr)
		{ PW_Utilities::Log("COULD NOT FIND CHARACTER OWNER"); return; }
	
	UCameraComponent* cameraComponent = ownerCharacter->GetCameraComponent();

	if (cameraComponent == nullptr)
		{ PW_Utilities::Log("NO CAMERA COMPONENT FOUND!"); return; }

	OnWeaponFireDelegate.Broadcast();
	int projectileCount = _weaponData->GetProjectileCount(_weaponFireMode);
	const float defaultProjectileDelay = _weaponData->GetProjectileDelay(_weaponFireMode);
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
	
	OnWeaponSuccessfulFire.Broadcast(hitResult, rayStart, rayDestination);
	QueueWeaponRecoil();
	
	if (isActorHit)
	{
		OnWeaponHit.Broadcast(hitResult);
	}
	
	ApplyDamage(hitResult);
	if (IsAmmoEmpty())
		QueueReloadWeapon(); 
}

void APW_WeaponObject::SimulateBulletSpread(FVector& rayDirection)
{
	const float weaponSpread = _weaponData->GetWeaponAccuracy(_weaponFireMode);
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
	if (!GetIsActive())
		return;
	
	const float fireRate = _weaponData->GetWeaponFireRate(_weaponFireMode);
	FTimerDelegate automaticFireDelegate;

	TWeakObjectPtr<APW_WeaponObject> weakThis = this;
	automaticFireDelegate.BindLambda([weakThis]()
		{ if (APW_WeaponObject* thisPtr = weakThis.Get())
			thisPtr->CoreFireSequence(); });
	
	GetWorld()->GetTimerManager().SetTimer(_fireTimerHandle, automaticFireDelegate, fireRate, false);
}

void APW_WeaponObject::TransferReserveAmmo()
{
	if (_weaponRuntimeData.CurrentReserveAmmo <= 0)
		return;

	const int ammoRequired = _weaponData->GetWeaponMagazineCapacity() - _weaponRuntimeData.CurrentAmmo;
	const int ammoToTransfer = FMath::Min(ammoRequired, _weaponRuntimeData.CurrentReserveAmmo);
	
	_weaponRuntimeData.CurrentAmmo += ammoToTransfer;
	_weaponRuntimeData.CurrentReserveAmmo -= ammoToTransfer;

	UpdateAmmo(_weaponRuntimeData.CurrentAmmo, _weaponRuntimeData.CurrentReserveAmmo);
}

void APW_WeaponObject::QueueWeaponRecoil()
{
	AActor* ownerActor = GetOwner();
	APW_Character* ownerCharacter = Cast<APW_Character>(ownerActor);

	if (ownerCharacter == nullptr)
		{ PW_Utilities::Log("COULD NOT FIND CHARACTER OWNER"); return; }

	const FVector2D minimumRecoil = _weaponData->GetMinimumWeaponRecoil(_weaponFireMode);
	const FVector2D maximumRecoil = _weaponData->GetMaximumWeaponRecoil(_weaponFireMode);
	const float recoilRecoverySpeed = _weaponData->GetWeaponRecoilSpeed(_weaponFireMode);

	const float recoilAmountX = FMath::RandRange(minimumRecoil.X, maximumRecoil.X);
	const float recoilAmountY = FMath::RandRange(minimumRecoil.Y, maximumRecoil.Y);
	const FVector2D recoilAmount = FVector2D(recoilAmountX, recoilAmountY);

	FLatentActionInfo latentActionInfo = FLatentActionInfo();
	latentActionInfo.CallbackTarget = this;
	latentActionInfo.ExecutionFunction = "CompleteWeaponRecoil";

	const FGuid UUID = FGuid::NewGuid();
	TUniquePtr<FRecoilAction> recoilAction = MakeUnique<FRecoilAction>
		(recoilRecoverySpeed, recoilAmount, ownerCharacter, latentActionInfo);
	
	FLatentActionManager& latentActionManager = GetWorld()->GetLatentActionManager();
	latentActionManager.AddNewAction(this, UUID.A, recoilAction.Get());
	
	recoilAction.Release();
}

void APW_WeaponObject::CompleteWeaponRecoil()
{
	
}

void APW_WeaponObject::QueueReloadWeapon()
{
	const bool cannotReload = IsMagazineFull()
		|| IsReserveAmmoEmpty()
		|| _weaponRuntimeData.IsReloading;

	if (!cannotReload)
		ReloadWeapon();
}

void APW_WeaponObject::ReloadWeapon()
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
	AActor* owner = GetOwner();
	const APW_Character* ownerCharacter = Cast<APW_Character>(owner);
	
	if (ownerCharacter == nullptr)
	{ PW_Utilities::Log("COULD NOT FIND CHARACTER OWNER"); return; } 
	
	if (_weaponData == nullptr)
	{ PW_Utilities::Log("NO WEAPON DATA FOUND!"); return; }
	
	AActor* hitActor = hitResult.GetActor();

	if (hitActor == nullptr)
	{ PW_Utilities::Log("HIT ACTOR NOT FOUND!"); return; }

	const float calculatedDamage = CalculateDamage(hitResult);
	
	_damageCauserComponent->Damage(hitActor, calculatedDamage);
}

float APW_WeaponObject::CalculateDamage(const FHitResult& hitResult)
{
	const float shotDistance = hitResult.Location.Distance(hitResult.TraceStart, hitResult.ImpactPoint);
	const float maximumDistance = _weaponData->GetWeaponMaximumDistance(_weaponFireMode);
	const float normalisedDistance = PWMath::Clamp01(1.0f - (shotDistance / maximumDistance));
	const float weaponDamage = _weaponData->GetWeaponDamage(_weaponFireMode);

	const UCurveFloat* fallOffCurve = _weaponData->GetWeaponFallOffCurve(_weaponFireMode);

	const float normalisedFallOff = fallOffCurve == nullptr
		                                ? normalisedDistance
		                                : fallOffCurve->GetFloatValue(normalisedDistance);
	
	const float calculatedDamage = weaponDamage * normalisedFallOff;
	return calculatedDamage;
}

bool APW_WeaponObject::CanFire()
{
	if (_weaponData == nullptr)
		{ PW_Utilities::Log("NO WEAPON DATA FOUND!"); return false; }

	if (_weaponRuntimeData.IsReloading)
		return false;
	
	if (_weaponRuntimeData.LastFiredTime < _weaponData->GetWeaponFireRate(_weaponFireMode))
		return false;

	// BELOW IS ALL TEMPORARY >>>>>>>>
	const ACharacter* characterObject = Cast<ACharacter>(GetOwner());
	
	if (characterObject == nullptr)
		{ PW_Utilities::Log("NO CHARACTER MOVEMENT COMPONENT FOUND!"); return false; }

	const UCharacterMovementComponent* characterMovement = characterObject->GetCharacterMovement();

	if (characterMovement == nullptr)
		{ PW_Utilities::Log("NO CHARACTER MOVEMENT FOUND"); return false; }

	const UActorComponent* pwCharacterMovement = characterObject->GetComponentByClass
		(UPW_CharacterMovementComponent::StaticClass());

	if (pwCharacterMovement == nullptr)
		{ PW_Utilities::Log("NO PW CHARACTER MOVEMENT FOUND!"); return false; }

	const UPW_CharacterMovementComponent* characterMovementComponent =
		Cast<UPW_CharacterMovementComponent>(pwCharacterMovement);

	if (characterMovementComponent == nullptr)
		{ PW_Utilities::Log("NO CHARACTER MOVEMENT COMPONENT FOUND!"); return false; }

	if (characterMovementComponent->GetReplicatedMovementData().IsSprinting)
		return false;

	if (characterMovement->IsFalling())
		return false;
	// ABOVE IS ALL TEMPORARY >>>>>>>>
	

	_weaponRuntimeData.LastFiredTime = 0.0f;
	return true;
}

void APW_WeaponObject::FireModeAim()
{
	if (_weaponFireMode == EFireMode::Aim)
		return;
	
	_weaponFireMode = EFireMode::Aim;
	
	AActor* ownerActor = GetOwner();
	const APW_Character* ownerCharacter = Cast<APW_Character>(ownerActor);

	if (ownerCharacter == nullptr)
		{ PW_Utilities::Log("COULD NOT FIND CHARACTER OWNER"); return; }

	UCameraComponent* cameraComponent = ownerCharacter->GetCameraComponent();
	UCharacterMovementComponent* characterMovement = ownerCharacter->GetCharacterMovement();

	if (cameraComponent == nullptr)
		{ PW_Utilities::Log("NO CAMERA COMPONENT FOUND!"); return; }

	if (characterMovement == nullptr)
		{ PW_Utilities::Log("NO CHARACTER MOVEMENT COMPONENT FOUND!"); return; }

	if (_weaponData == nullptr)
		{ PW_Utilities::Log("NO WEAPON DATA FOUND!"); return; }

	const float fovModifier = _weaponData->GetFieldOfViewModifier();

	if (fovModifier == 0.0f)
		{ PW_Utilities::Log("FOV MODIFIER IS 0!"); return; }

	OnWeaponFireModeAim.Broadcast();
	
	const float currentFov = cameraComponent->FieldOfView;
	const float speedModifier = _weaponData->GetMovementSpeedModifier();

	cameraComponent->SetFieldOfView(currentFov * fovModifier);
	characterMovement->MaxWalkSpeed *= speedModifier;
}

void APW_WeaponObject::FireModeHip()
{
	if (_weaponFireMode == EFireMode::Hip)
		return;
	
	_weaponFireMode = EFireMode::Hip;
	
	AActor* ownerActor = GetOwner();
	const APW_Character* ownerCharacter = Cast<APW_Character>(ownerActor);

	if (ownerCharacter == nullptr)
		{ PW_Utilities::Log("COULD NOT FIND CHARACTER OWNER"); return; }

	UCameraComponent* cameraComponent = ownerCharacter->GetCameraComponent();
	UCharacterMovementComponent* characterMovement = ownerCharacter->GetCharacterMovement();

	if (cameraComponent == nullptr)
		{ PW_Utilities::Log("NO CAMERA COMPONENT FOUND!"); return; }

	if (characterMovement == nullptr)
		{ PW_Utilities::Log("NO CHARACTER MOVEMENT COMPONENT FOUND!"); return; }

	if (_weaponData == nullptr)
		{ PW_Utilities::Log("NO WEAPON DATA FOUND!"); return; }
  
	const float fovModifier = _weaponData->GetFieldOfViewModifier();

	if (fovModifier == 0.0f)
		{ PW_Utilities::Log("FOV MODIFIER IS 0!"); return; }

	OnWeaponFireModeHip.Broadcast();
	
	const float currentFov = cameraComponent->FieldOfView;
	const float speedModifier = _weaponData->GetMovementSpeedModifier();

	cameraComponent->SetFieldOfView(currentFov / fovModifier);
	characterMovement->MaxWalkSpeed /= speedModifier;
}

void APW_WeaponObject::UpdateAmmo(int currentAmmo, int currentReserveAmmo)
{
	GetOwner()->HasAuthority()
	? LocalUpdateAmmo(currentAmmo, currentReserveAmmo)
	: ServerUpdateAmmo(currentAmmo, currentReserveAmmo);
}

void APW_WeaponObject::LocalUpdateAmmo(int currentAmmo, int currentReserveAmmo)
{
	_weaponRuntimeData.CurrentAmmo = PWMath::ClampZero(currentAmmo);
	_weaponRuntimeData.CurrentReserveAmmo = PWMath::ClampZero(currentReserveAmmo);
}

void APW_WeaponObject::ServerUpdateAmmo_Implementation(int currentAmmo, int currentReserveAmmo)
{
	if (GetOwner()->HasAuthority())
		LocalUpdateAmmo(currentAmmo, currentReserveAmmo);
}

