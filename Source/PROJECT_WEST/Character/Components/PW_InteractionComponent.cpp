// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_InteractionComponent.h"
#include "PROJECT_WEST/Interfaces//PW_InteractableInterface.h"
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

void UPW_InteractionComponent::TryClearLastInteractable()
{
	if (_lastInteractable && !_lastInteractable->IsInteracting_Implementation())
	{
		_lastInteractable->EndFocus_Implementation();
		_lastInteractable = nullptr;
	}
}

void UPW_InteractionComponent::TraceForInteractable()
{
	if(!_ownerCharacter) return;
	if(_lastInteractable && _lastInteractable->IsInteracting_Implementation()) return;
	
	UCameraComponent* _cameraComponent = _ownerCharacter->GetCameraComponent();
	if (!_cameraComponent) return;

	IPW_InteractableInterface* Interactable = nullptr;
	FVector direction = _cameraComponent->GetForwardVector();
	FVector start = _cameraComponent->GetComponentLocation();
	FVector destination = start + (direction * 1000.0f);
	
	TArray< TEnumAsByte<EObjectTypeQuery> > objectTypes;
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Visibility));
	
	float halfHeight = 10;
	FHitResult hitResult;
	
	if (UKismetSystemLibrary::BoxTraceSingleForObjects(GetWorld(), start, destination, FVector(halfHeight, halfHeight, halfHeight), FRotator(),
	objectTypes, false, TArray<AActor*>(), EDrawDebugTrace::None, hitResult,
		true, FLinearColor::Red, FLinearColor::Green, 5.0f))
	{
		Interactable = Cast<IPW_InteractableInterface>(hitResult.GetActor());
		if (Interactable)
		{
			if (Interactable != _lastInteractable)
			{
				TryClearLastInteractable();
				_lastInteractable = Interactable;
				_lastInteractable->StartFocus_Implementation();
				DEBUG_STRING( "Interactable Start Focus");
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

void UPW_InteractionComponent::TryStartInteractWithInteractable()
{
	if (_lastInteractable && !_lastInteractable->IsInteracting_Implementation())
	{
		_lastInteractable->EndFocus_Implementation();
		_lastInteractable->StartInteract_Implementation(GetOwner());
	}
}

void UPW_InteractionComponent::TryEndInteractWithInteractable()
{
	if (_lastInteractable && _lastInteractable->IsInteracting_Implementation())
	{
		_lastInteractable->EndInteract_Implementation();
		_lastInteractable = nullptr;
	}
}

