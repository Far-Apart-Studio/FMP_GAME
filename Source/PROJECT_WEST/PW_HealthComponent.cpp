// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_HealthComponent.h"

#include "DebugMacros.h"
#include "PW_Utilities.h"
#include "Net/UnrealNetwork.h"

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
	{
		ownerActor->OnTakeAnyDamage.AddDynamic(this, &UPW_HealthComponent::TakeDamage);
		PW_Utilities::Log("Health Component Successfully Initialized");
	}
}

void UPW_HealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPW_HealthComponent, _currentHealth);
}

void UPW_HealthComponent::OnRep_OnHealthChange()
{
	FString healthString = GetOwner()->HasAuthority() ? "Server" : "Client";
	DEBUG_STRING("HEALTH CHANGED!" + healthString);

	if (_currentHealth <= _minimumHealth)
		OnDeath.Broadcast();
}

void UPW_HealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPW_HealthComponent::TakeDamage(AActor* damageActor, float damageAmount, const UDamageType*
	damageType, AController* instigatedBy, AActor* damageCauser)
{
	FString healthString = GetOwner()->HasAuthority() ? "Server" : "Client";
	DEBUG_STRING("HEALTH CHANGED!" + healthString);
	
	if (damageAmount <= 0.0f)
		return;

	_currentHealth = FMath::Clamp(_currentHealth - damageAmount, _minimumHealth, _maxHealth);

	if (_currentHealth == _minimumHealth)
		OnDeath.Broadcast();
}
