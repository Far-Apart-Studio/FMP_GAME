// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_HealthComponent.h"
#include "PW_Character.h"
#include "Net/UnrealNetwork.h"

UPW_HealthComponent::UPW_HealthComponent():
	_characterOwner(nullptr),
	_fallDamageData()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPW_HealthComponent::BeginPlay()
{
	Super::BeginPlay();
	SetIsReplicated( true );
	_currentHealth = _defaultHealth;

	AActor* ownerActor = GetOwner();
	_characterOwner = Cast<APW_Character>(ownerActor);

	if (_characterOwner && _characterOwner->HasAuthority())
	{
		_characterOwner->OnTakeAnyDamage.AddDynamic(this, &UPW_HealthComponent::TakeDamage);
		_regenerationHandle.RegenerationMethod.BindUObject(this, &UPW_HealthComponent::RecoverHealth);
		_regenerationHandle.RegenerationCondition.BindUObject(this, &UPW_HealthComponent::CanRecoverHealth);
		_characterOwner->LandedDelegate.AddDynamic(this, &UPW_HealthComponent::ApplyLandedDamage);
	}
}

void UPW_HealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPW_HealthComponent, _currentHealth);
	DOREPLIFETIME(UPW_HealthComponent, _isAlive);
	DOREPLIFETIME(UPW_HealthComponent, _isInvulnerable);
	DOREPLIFETIME(UPW_HealthComponent, _regenerationHandle);
}

void UPW_HealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	_lastTakenDamage += DeltaTime;
	_regenerationHandle.Regenerate(_currentHealth, DeltaTime);
}

void UPW_HealthComponent::RecoverHealth(float recoverValue)
{
	OnHealingReceivedLocal.Broadcast(GetOwner(), nullptr, nullptr, recoverValue);
	OnHealthChangedLocal.Broadcast();
	GetOwnerRole() == ROLE_Authority ? LocalRecoverHealth(recoverValue) : ServerRecoverHealth(recoverValue);
}

void UPW_HealthComponent::LocalRecoverHealth(float recoverValue)
{
	_currentHealth = FMath::Clamp(_currentHealth + recoverValue, _minimumHealth, _maxHealth);
	MulticastHealthModified(GetOwner(), nullptr, nullptr, recoverValue);
}

void UPW_HealthComponent::ServerRecoverHealth_Implementation(float recoverValue)
{
	if (GetOwnerRole() == ROLE_Authority)
		LocalRecoverHealth(recoverValue);
}

void UPW_HealthComponent::MulticastHealthModified_Implementation(AActor* ownerActor, AActor* damageCauser,
	AController* damageCauserController, float modificationAmount)
{	
	OnHealthChangedGlobal.Broadcast();

	(modificationAmount < 0.0f ? OnDamageReceivedGlobal : OnHealingReceivedGlobal).Broadcast
	(ownerActor, damageCauser, damageCauserController, modificationAmount);

	if (_currentHealth == _minimumHealth)
	{
		_isAlive = false;
		OnDeathGlobal.Broadcast(ownerActor, damageCauser, damageCauserController);
	}
}

void UPW_HealthComponent::TakeDamage(AActor* ownerActor, float damageAmount,
	const UDamageType* damageType, AController* instigatedBy, AActor* damageCauser)
{
	if (!CanReceiveDamage(damageAmount))
		return;

	OnDamageReceivedLocal.Broadcast(ownerActor, damageCauser, instigatedBy, damageAmount);
	OnHealthChangedLocal.Broadcast();
	
	GetOwnerRole() == ROLE_Authority ? LocalTakeDamage(ownerActor, damageAmount, damageType, instigatedBy, damageCauser)
	: ServerTakeDamage(ownerActor, damageAmount, damageType, instigatedBy, damageCauser);
}

void UPW_HealthComponent::LocalTakeDamage(AActor* ownerActor, float damageAmount,
                                          const UDamageType* damageType,AController* instigatedBy, AActor* damageCauser)
{
	AActor* actorOwner = GetOwner();
	_currentHealth = FMath::Clamp(_currentHealth - damageAmount, _minimumHealth, _maxHealth);

	MulticastHealthModified(actorOwner, damageCauser, instigatedBy, -damageAmount);

	if (_currentHealth == _minimumHealth)
	{
		_isAlive = false;
		OnDeathGlobal.Broadcast(actorOwner, damageCauser, instigatedBy);
	}
}

void UPW_HealthComponent::SetIsInvulnerable(bool isInvulnerable)
{
	GetOwnerRole() == ROLE_Authority ? LocalSetIsInvulnerable(isInvulnerable)
	: ServerSetIsInvulnerable(isInvulnerable);
}

void UPW_HealthComponent::LocalSetIsInvulnerable(bool isInvulnerable)
{
	_isInvulnerable = isInvulnerable;
	OnInvulnerabilityGlobal.Broadcast();
}

void UPW_HealthComponent::ServerSetIsInvulnerable_Implementation(bool isInvulnerable)
{
	if (GetOwnerRole() == ROLE_Authority)
		LocalSetIsInvulnerable(isInvulnerable);
}

void UPW_HealthComponent::ServerTakeDamage_Implementation(AActor* ownerActor, float damageAmount,
     const UDamageType* damageType, AController* instigatedBy, AActor* damageCauser)
{
	if (GetOwnerRole() == ROLE_Authority)
		LocalTakeDamage(ownerActor, damageAmount, damageType, instigatedBy, damageCauser);
}

void UPW_HealthComponent::SetCanNaturallyRegenerate(bool canNaturallyRegenerate)
{
	GetOwnerRole() == ROLE_Authority ? LocalSetCanNaturallyRegenerate(canNaturallyRegenerate)
	: ServerSetCanNaturallyRegenerate(canNaturallyRegenerate);
}

void UPW_HealthComponent::LocalSetCanNaturallyRegenerate(bool canNaturallyRegenerate)
{
	_regenerationHandle.AllowRegeneration = canNaturallyRegenerate;
}

void UPW_HealthComponent::ServerSetCanNaturallyRegenerate_Implementation(bool canNaturallyRegenerate)
{
	if (GetOwnerRole() == ROLE_Authority)
		LocalSetCanNaturallyRegenerate(canNaturallyRegenerate);
}

void UPW_HealthComponent::ApplyLandedDamage(const FHitResult& hitResult)
{
	if (!CanReceiveLandedDamage())
		return;
	
	const FVector characterVelocity = _characterOwner->GetVelocity();
	const float absCharacterVelocity = FMath::Abs(characterVelocity.Z);

	if (absCharacterVelocity < _fallDamageData.MinimumVelocity)
		return;
	
	const float clampedVelocity = FMath::Clamp(absCharacterVelocity,
	_fallDamageData.MinimumVelocity, _fallDamageData.MaximumVelocity);

	const float normalisedVelocity = clampedVelocity / _fallDamageData.MaximumVelocity;
	const float damageAmount = clampedVelocity * _fallDamageData.DamageMultiplier;
	float finalDamage = damageAmount;

	if (_fallDamageData.FallDamageCurve != nullptr)
	{
		const float curveValue = _fallDamageData.FallDamageCurve->GetFloatValue(normalisedVelocity);
		finalDamage = damageAmount * curveValue;
	}
	
	TakeDamage(_characterOwner, finalDamage, nullptr,
		_characterOwner->GetController() ,_characterOwner);

	OnFallDamageReceived.Broadcast();
}

bool UPW_HealthComponent::CanReceiveLandedDamage()
{
	return _fallDamageData.AllowFallDamage;
}

bool UPW_HealthComponent::CanReceiveDamage(float damageAmount) const
{
	return damageAmount > 0.0f
	 && _isAlive
	 && !_isInvulnerable;
}

bool UPW_HealthComponent::CanRecoverHealth()
{
	return _lastTakenDamage >= _combatRecoveryDelay && _isAlive;
}
