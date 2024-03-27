// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_SphereDamageCauser.h"
#include "Kismet/KismetSystemLibrary.h"

void UPW_SphereDamageCauser::GetActorsWithinBounds()
{
	_actorsDamaged.Empty();
	
	AActor* ownerActor = GetOwner();
	const FVector actorPosition = ownerActor->GetActorLocation();
	const TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes = { UEngineTypes::ConvertToObjectType(ECC_Visibility) };
	const TArray<AActor*> actorsToIgnore;

	UKismetSystemLibrary::SphereOverlapActors(
		ownerActor,
		actorPosition,
		_damageRadius,
		objectTypes,
		nullptr,
		actorsToIgnore,
		_actorsDamaged);
}

void UPW_SphereDamageCauser::DamageActorsWithinBounds(float damageAmount)
{
	AActor* ownerActor = GetOwner();
	AController* ownerController = ownerActor->GetInstigatorController();

	GetActorsWithinBounds();

	for (const auto damagedActors : _actorsDamaged)
		damagedActors->TakeDamage(damageAmount, FDamageEvent(), ownerController, ownerActor);

#if ENABLE_DRAW_DEBUG
	UKismetSystemLibrary::DrawDebugSphere(ownerActor, ownerActor->GetActorLocation(),
		_damageRadius, 12, FLinearColor::Red, 5.0f, 1.0f);
#endif
	
}
