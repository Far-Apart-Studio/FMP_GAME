// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_InteractionComponent.h"
#include "PROJECT_WEST/Interfaces/PW_InteractableInterface.h"
#include <Kismet/KismetMathLibrary.h>
#include "PROJECT_WEST/PW_Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Camera/CameraComponent.h"
#include "PROJECT_WEST/DebugMacros.h"

UPW_InteractionComponent::UPW_InteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	_traceDistance = 500;
}

void UPW_InteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	SetIsReplicated( true );
	
	_ownerCharacter = Cast<APW_Character>(GetOwner());
	if(_ownerCharacter && _ownerCharacter->IsLocallyControlled())
	{
		_ownerCharacter->OnInteractButtonPressed.AddDynamic(this, &UPW_InteractionComponent::TryToggleInteract);
	}
}

void UPW_InteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if(!_ownerCharacter || !_ownerCharacter->IsLocallyControlled()) return;
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TraceForInteractable();
}

void UPW_InteractionComponent::TraceForInteractable()
{
	if(!_ownerCharacter) return;
	if(_lastIntractableActor && IPW_InteractableInterface::Execute_IsInteracting(_lastIntractableActor)) return;
	
	UCameraComponent* _cameraComponent = _ownerCharacter->GetCameraComponent();
	if (!_cameraComponent) return;
	
	FVector direction = _cameraComponent->GetForwardVector();
	FVector start = _cameraComponent->GetComponentLocation();
	FVector destination = start + (direction * _traceDistance);
	
	// Interactable
	ETraceTypeQuery traceType = UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1);
	
	float halfHeight = 10;
	FHitResult hitResult;
	
	if (UKismetSystemLibrary::BoxTraceSingle(GetWorld(), start, destination, FVector(halfHeight, halfHeight, halfHeight), FRotator(),
	traceType, false, TArray<AActor*>(), EDrawDebugTrace::None, hitResult,
		true, FLinearColor::Red, FLinearColor::Green, 5.0f))
	{
		if (hitResult.GetActor() && hitResult.GetActor()->GetClass()->ImplementsInterface(UPW_InteractableInterface::StaticClass()))
		{
			if (_lastIntractableActor != hitResult.GetActor())
			{
				TryClearLastInteractable();
				_lastIntractableActor = hitResult.GetActor();
				IPW_InteractableInterface::Execute_StartFocus(_lastIntractableActor,GetOwner());
			}
		}
		else
		{
			TryClearLastInteractable();
		}
	}
	else
	{
		TryClearLastInteractable();
	}
}

void UPW_InteractionComponent::TryClearLastInteractable()
{
	if (_lastIntractableActor && !IPW_InteractableInterface::Execute_IsInteracting(_lastIntractableActor))
	{
		IPW_InteractableInterface::Execute_EndFocus(_lastIntractableActor,GetOwner());
		_lastIntractableActor = nullptr;
	}
}

void UPW_InteractionComponent::TryStartInteractWithInteractable()
{
	if (_lastIntractableActor && !IPW_InteractableInterface::Execute_IsInteracting(_lastIntractableActor))
	{
		IPW_InteractableInterface::Execute_EndFocus(_lastIntractableActor,GetOwner());
		IPW_InteractableInterface::Execute_StartInteract(_lastIntractableActor, GetOwner());

		if (IPW_InteractableInterface::Execute_HasServerInteraction(_lastIntractableActor))
		{
			StartServerInteractWithInteractable(_lastIntractableActor, GetOwner());
		}
	}
}

void UPW_InteractionComponent::TryEndInteractWithInteractable()
{
	if (_lastIntractableActor && IPW_InteractableInterface::Execute_IsInteracting(_lastIntractableActor))
	{
		IPW_InteractableInterface::Execute_EndInteract(_lastIntractableActor);
		
		if (IPW_InteractableInterface::Execute_HasServerInteraction(_lastIntractableActor))
		{
			EndServerInteractWithInteractable( _lastIntractableActor);
		}
		
		_lastIntractableActor = nullptr;
	}
}

void UPW_InteractionComponent::StartServerInteractWithInteractable(AActor* _interactedActor,AActor* _interactableOwner)
{
	GetOwner()->HasAuthority() ? LocalStartInteractWithInteractable(_interactedActor, _interactableOwner) : ServerStartInteractWithInteractable(_interactedActor, _interactableOwner);
}

void UPW_InteractionComponent::LocalStartInteractWithInteractable(AActor* _interactedActor,AActor* _interactableOwner)
{
	IPW_InteractableInterface::Execute_ServerStartInteract(_interactedActor, _interactableOwner);
}

void UPW_InteractionComponent::EndServerInteractWithInteractable(AActor* _interactedActor)
{
	GetOwner()->HasAuthority() ? LocalEndInteractWithInteractable(_interactedActor) : ServerEndInteractWithInteractable(_interactedActor);
}

void UPW_InteractionComponent::ServerEndInteractWithInteractable_Implementation(AActor* _interactedActor)
{
	if (!GetOwner()->HasAuthority()) return;
	LocalEndInteractWithInteractable(_interactedActor);
}

void UPW_InteractionComponent::LocalEndInteractWithInteractable(AActor* _interactedActor)
{
	IPW_InteractableInterface::Execute_ServerStopInteract(_interactedActor);
}

void UPW_InteractionComponent::ServerStartInteractWithInteractable_Implementation(AActor* _interactedActor,AActor* _interactableOwner)
{
	if (!GetOwner()->HasAuthority()) return;
	LocalStartInteractWithInteractable(_interactedActor, _interactableOwner);
}

void UPW_InteractionComponent::TryToggleInteract()
{
	if(_lastIntractableActor && IPW_InteractableInterface::Execute_IsInteracting(_lastIntractableActor))
	{
		TryEndInteractWithInteractable();
	}
	else
	{
		TryStartInteractWithInteractable();
	}
}
