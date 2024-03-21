// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_DamageCauserComponent.h"

UPW_DamageCauserComponent::UPW_DamageCauserComponent()
{

}

void UPW_DamageCauserComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPW_DamageCauserComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPW_DamageCauserComponent::Damage(AActor* damageableActor, float damageAmount)
{
	if (!damageableActor)
		return;

	GetOwnerRole() == ROLE_Authority
	? LocalDamage(damageableActor, damageAmount)
	: ServerDamage(damageableActor, damageAmount);
}

void UPW_DamageCauserComponent::LocalDamage(AActor* damageableActor, float damageAmount)
{
	if (damageableActor)
	{
		AActor* damageCauser = GetOwner();
		const FDamageEvent damageEvent = FDamageEvent();
		damageableActor->TakeDamage(damageAmount, damageEvent, damageCauser->GetInstigatorController(), damageCauser);
	}
}

void UPW_DamageCauserComponent::ServerDamage_Implementation(AActor* damageableActor, float damageAmount)
{
	if (GetOwnerRole() == ROLE_Authority)
		LocalDamage(damageableActor, damageAmount);
}



