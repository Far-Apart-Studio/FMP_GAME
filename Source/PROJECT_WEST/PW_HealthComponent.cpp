// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_HealthComponent.h"

#include "DebugMacros.h"
#include "PW_Utilities.h"
#include "Net/UnrealNetwork.h"

UPW_HealthComponent::UPW_HealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated( true );
}

void UPW_HealthComponent::BeginPlay()
{
	Super::BeginPlay();
	_currentHealth = _defaultHealth;
	AActor* ownerActor = GetOwner();

	if (ownerActor && ownerActor->HasAuthority())
	{
		ownerActor->OnTakeAnyDamage.AddDynamic(this, &UPW_HealthComponent::TakeDamage);
		//PW_Utilities::Log("Health Component Successfully Initialized");
		//DEBUG_STRING( "Health Component Successfully Initialized" );
	}
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
	{
		OnHealthChanged.Broadcast();
	}
}

void UPW_HealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
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
