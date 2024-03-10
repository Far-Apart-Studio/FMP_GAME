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
}

void UPW_InteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	SetIsReplicated( true );
	
	_ownerCharacter = Cast<APW_Character>(GetOwner());
	if(_ownerCharacter && _ownerCharacter->IsLocallyControlled())
	{
		_ownerCharacter->OnStartInteractButtonPressed.AddDynamic(this, &UPW_InteractionComponent::TryStartInteractWithInteractable);
		_ownerCharacter->OnEndInteractButtonPressed.AddDynamic(this, &UPW_InteractionComponent::TryEndInteractWithInteractable);
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
	if(_lastInteractableActor && IPW_InteractableInterface::Execute_IsInteracting(_lastInteractableActor)) return;
	
	UCameraComponent* _cameraComponent = _ownerCharacter->GetCameraComponent();
	if (!_cameraComponent) return;

	IPW_InteractableInterface* Interactable = nullptr;
	FVector direction = _cameraComponent->GetForwardVector();
	FVector start = _cameraComponent->GetComponentLocation();
	FVector destination = start + (direction * 1000.0f);
	
	TArray< TEnumAsByte<EObjectTypeQuery> > objectTypes;
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Visibility));
	//objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	//objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	
	float halfHeight = 10;
	FHitResult hitResult;
	
	if (UKismetSystemLibrary::BoxTraceSingleForObjects(GetWorld(), start, destination, FVector(halfHeight, halfHeight, halfHeight), FRotator(),
	objectTypes, false, TArray<AActor*>(), EDrawDebugTrace::None, hitResult,
		true, FLinearColor::Red, FLinearColor::Green, 5.0f))
	{
		if (hitResult.GetActor()->GetClass()->ImplementsInterface(UPW_InteractableInterface::StaticClass()))
		{
			if (_lastInteractableActor != hitResult.GetActor())
			{
				TryClearLastInteractable();
				_lastInteractableActor = hitResult.GetActor();
				IPW_InteractableInterface::Execute_StartFocus(_lastInteractableActor);
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
	if (_lastInteractableActor && !IPW_InteractableInterface::Execute_IsInteracting(_lastInteractableActor))
	{
		IPW_InteractableInterface::Execute_EndFocus(_lastInteractableActor);
		_lastInteractableActor = nullptr;
	}
}

void UPW_InteractionComponent::TryStartInteractWithInteractable()
{
	if (_lastInteractableActor && !IPW_InteractableInterface::Execute_IsInteracting(_lastInteractableActor))
	{
		IPW_InteractableInterface::Execute_EndFocus(_lastInteractableActor);
		IPW_InteractableInterface::Execute_StartInteract(_lastInteractableActor, GetOwner());

		if (IPW_InteractableInterface::Execute_HasServerInteraction(_lastInteractableActor))
		{
			StartInteractWithInteractable(_lastInteractableActor, GetOwner());
		}
	}
}

void UPW_InteractionComponent::TryEndInteractWithInteractable()
{
	if (_lastInteractableActor && IPW_InteractableInterface::Execute_IsInteracting(_lastInteractableActor))
	{
		IPW_InteractableInterface::Execute_EndInteract(_lastInteractableActor);
		_lastInteractableActor = nullptr;
	}
}

void UPW_InteractionComponent::StartInteractWithInteractable(AActor* _interatedActor,AActor* _interactableOwner)
{
	if (GetOwner()->HasAuthority())
	{
		LocalStartInteractWithInteractable(_interatedActor, _interactableOwner);
	}
	else
	{
		ServerStartInteractWithInteractable(_interatedActor, _interactableOwner);
	}
}

void UPW_InteractionComponent::LocalStartInteractWithInteractable(AActor* _interatedActor,AActor* _interactableOwner)
{
	IPW_InteractableInterface::Execute_ServerStartInteract(_interatedActor, _interactableOwner);
}

void UPW_InteractionComponent::ServerStartInteractWithInteractable_Implementation(AActor* _interatedActor,AActor* _interactableOwner)
{
	if (!GetOwner()->HasAuthority()) return;
	LocalStartInteractWithInteractable(_interatedActor, _interactableOwner);
}
