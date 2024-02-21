// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_HealthComponent.h"
#include "Net/UnrealNetwork.h"

UPW_HealthComponent::UPW_HealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPW_HealthComponent::BeginPlay()
{
	Super::BeginPlay();
	SetIsReplicated( true );
	_currentHealth = _defaultHealth;
	AActor* ownerActor = GetOwner();

	if (ownerActor && ownerActor->HasAuthority())
		ownerActor->OnTakeAnyDamage.AddDynamic(this, &UPW_HealthComponent::TakeDamage);
}

void UPW_HealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPW_HealthComponent, _currentHealth);
	DOREPLIFETIME(UPW_HealthComponent, _isAlive);
}

void UPW_HealthComponent::OnRep_OnHealthChange(float lastHealth)
{
	if (_currentHealth < lastHealth)
		OnHealthChanged.Broadcast();
}

void UPW_HealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPW_HealthComponent::RecoverHealth(float recoverValue)
{
	_currentHealth = FMath::Clamp(_currentHealth + recoverValue, _minimumHealth, _maxHealth);
}

void UPW_HealthComponent::TakeDamage(AActor* damageActor, float damageAmount, const UDamageType*damageType, AController* instigatedBy, AActor* damageCauser)
{
	if (damageAmount <= 0.0f || !_isAlive)
		return;

	_currentHealth = FMath::Clamp(_currentHealth - damageAmount, _minimumHealth, _maxHealth);

	OnHealthChanged.Broadcast();

	if (_currentHealth == _minimumHealth)
	{
		_isAlive = false;
		OnDeath.Broadcast(damageCauser, instigatedBy );
	}
}
