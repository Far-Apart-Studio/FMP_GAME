// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_HealthComponent.h"

UPW_HealthComponent::UPW_HealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPW_HealthComponent::BeginPlay()
{
	Super::BeginPlay();
	_currentHealth = _defaultHealth;
	AActor* ownerActor = GetOwner();

	if (ownerActor)
		ownerActor->OnTakeAnyDamage.AddDynamic(this, &UPW_HealthComponent::TakeDamage);
	
}

void UPW_HealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPW_HealthComponent::TakeDamage(AActor* damageActor, float damageAmount, const UDamageType*
	damageType, AController* instigatedBy, AActor* damageCauser)
{
	if (damageAmount <= 0.0f)
		return;

	_currentHealth = FMath::Clamp(_currentHealth - damageAmount, _minimumHealth, _maxHealth);

	if (_currentHealth == _minimumHealth)
		OnDeath.Broadcast();
}
