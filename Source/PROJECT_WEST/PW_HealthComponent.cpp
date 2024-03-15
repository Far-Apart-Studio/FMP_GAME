// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_HealthComponent.h"

#include "DebugMacros.h"
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

	if (ownerActor)
	{
		ownerActor->OnTakeAnyDamage.AddDynamic(this, &UPW_HealthComponent::TakeDamage);
		_regenerationHandle.RegenerationMethod.BindUObject(this, &UPW_HealthComponent::RecoverHealth);
		_regenerationHandle.RegenerationCondition.BindUObject(this, &UPW_HealthComponent::CanRecoverHealth);
	}

	if (_characterOwner)
		_characterOwner->LandedDelegate.AddDynamic(this, &UPW_HealthComponent::ApplyLandedDamage);
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
	
	if (GetOwnerRole() == ROLE_Authority)
		_regenerationHandle.Increase(_currentHealth, DeltaTime);
}

void UPW_HealthComponent::RecoverHealth(float recoverValue)
{
	OnHealingReceivedServer.Broadcast(GetOwner(), nullptr, nullptr, recoverValue);
	OnHealthChangedServer.Broadcast();
	LocalRecoverHealth(recoverValue);
}

void UPW_HealthComponent::LocalRecoverHealth(float recoverValue)
{
	_currentHealth = FMath::Clamp(_currentHealth + recoverValue, _minimumHealth, _maxHealth);
}

void UPW_HealthComponent::TakeDamage(AActor* ownerActor, float damageAmount,
	const UDamageType* damageType, AController* instigatedBy, AActor* damageCauser)
{
	if (!CanReceiveDamage(damageAmount))
		return;
	
	OnDamageReceivedServer.Broadcast(ownerActor, damageCauser, instigatedBy, damageAmount);
	OnHealthChangedServer.Broadcast();
	
	const float lastHealth = _currentHealth;

	_currentHealth = FMath::Clamp(_currentHealth - damageAmount, _minimumHealth, _maxHealth);

	const float modificationAmount = _currentHealth - lastHealth;
	(modificationAmount < 0.0f ? OnDamageReceivedGlobal : OnHealingReceivedGlobal).Broadcast
	(GetOwner(), damageCauser, instigatedBy, modificationAmount);

	if (_currentHealth == _minimumHealth)
	{
		_isAlive = false;
		AActor* actorOwner = GetOwner();
		OnDeathServer.Broadcast(actorOwner, damageCauser, instigatedBy);
		OnDeathGlobal.Broadcast(actorOwner, damageCauser, instigatedBy);
	}
	
	_regenerationHandle.Reset();
}

void UPW_HealthComponent::SetIsInvulnerable(bool isInvulnerable)
{
	LocalSetIsInvulnerable(isInvulnerable);
}

void UPW_HealthComponent::LocalSetIsInvulnerable(bool isInvulnerable)
{
	_isInvulnerable = isInvulnerable;
	OnInvulnerabilityGlobal.Broadcast();
}

void UPW_HealthComponent::SetCanNaturallyRegenerate(bool canNaturallyRegenerate)
{
	LocalSetCanNaturallyRegenerate(canNaturallyRegenerate);
}

void UPW_HealthComponent::LocalSetCanNaturallyRegenerate(bool canNaturallyRegenerate)
{
	_regenerationHandle.AllowRegeneration = canNaturallyRegenerate;
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

void UPW_HealthComponent::OnRep_HealthChanged(float lastHealth)
{
	float modificationAmount = _currentHealth - lastHealth;

	OnHealthChangedGlobal.Broadcast();

	(modificationAmount < 0.0f ? OnDamageReceivedGlobal : OnHealingReceivedGlobal).Broadcast
	(GetOwner(), nullptr, nullptr, modificationAmount);

	if (_currentHealth == _minimumHealth)
	{
		_isAlive = false;
		OnDeathGlobal.Broadcast(GetOwner(), nullptr, nullptr);
	}
}

bool UPW_HealthComponent::CanReceiveDamage(float damageAmount) const
{
	return damageAmount > 0.0f
	 && _isAlive
	 && !_isInvulnerable;
}

bool UPW_HealthComponent::CanRecoverHealth()
{
	return _isAlive;
}
