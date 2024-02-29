// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_DistanceUnloaderComponent.h"

#include "Kismet/GameplayStatics.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/PW_Character.h"

UPW_DistanceUnloaderComponent::UPW_DistanceUnloaderComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	_isUnLoaded = false;
	_distanceToUnload = 1000.0f;
}

void UPW_DistanceUnloaderComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPW_DistanceUnloaderComponent::TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction)
{
	if (!GetOwner()->HasAuthority())
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
		APW_Character* character = Cast<APW_Character>(actor);
		if (character)
		{
			float distance = FVector::Dist(character->GetActorLocation(), GetOwner()->GetActorLocation());
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
