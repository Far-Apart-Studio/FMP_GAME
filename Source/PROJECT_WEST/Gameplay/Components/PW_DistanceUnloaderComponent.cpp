// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_DistanceUnloaderComponent.h"

#include "Kismet/GameplayStatics.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/PW_HealthComponent.h"

UPW_DistanceUnloaderComponent::UPW_DistanceUnloaderComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	_isUnLoaded = false;
	_canUnload = false;
	_distanceToUnload = 10000.0f;
}

void UPW_DistanceUnloaderComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPW_DistanceUnloaderComponent::TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction)
{
	if (!GetOwner()->HasAuthority() || !_canUnload)
		return;
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	HandleDistanceUnloading();
}

void UPW_DistanceUnloaderComponent::HandleDistanceUnloading()
{
	if(_isUnLoaded)
		return;
	
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APW_Character::StaticClass(), actors);
	
	for (AActor* actor : actors)
	{
		if (const APW_Character* character = Cast<APW_Character>(actor))
		{
			const UPW_HealthComponent* healthComponent = character->FindComponentByClass<UPW_HealthComponent>();
			if (healthComponent && !healthComponent->IsAlive())
				continue;
			
			const float distance = FVector::Dist(character->GetActorLocation(), GetOwner()->GetActorLocation());
			if (distance < _distanceToUnload)
				return;
		}
	}
	
	UnLoad();
}

void UPW_DistanceUnloaderComponent::UnLoad()
{
	_isUnLoaded = true;
	_onUnloaded.Broadcast(GetOwner());
	GetOwner()->Destroy();
}

void UPW_DistanceUnloaderComponent::SetCanUnload(bool status)
{
	_canUnload = status;
}