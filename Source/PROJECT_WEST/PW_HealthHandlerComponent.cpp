// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_HealthHandlerComponent.h"

UPW_HealthHandlerComponent::UPW_HealthHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPW_HealthHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
	_currentHealth = _defaultHealth;
	AActor* ownerActor = GetOwner();

	if (ownerActor)
		ownerActor->OnTakeAnyDamage.AddDynamic(this, &UPW_HealthHandlerComponent::TakeDamage);
	
}

void UPW_HealthHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPW_HealthHandlerComponent::TakeDamage(AActor* damageActor, float damageAmount, const UDamageType*
	damageType, AController* instigatedBy, AActor* damageCauser)
{
	if (damageAmount <= 0.0f)
		return;

	_currentHealth = FMath::Clamp(_currentHealth - damageAmount, _minimumHealth, _maxHealth);

	if (_currentHealth == _minimumHealth)
		OnDeath.Broadcast();
}
