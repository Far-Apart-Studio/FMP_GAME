// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_MaterialEffectComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "PROJECT_WEST/DebugMacros.h"

UPW_MaterialEffectComponent::UPW_MaterialEffectComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	_effectName = "Make sure to set the effect name";
	_fadeSpeed = 0.1f;
	_autoGenerateMeshComponents = true;
	_startingEffectValue = 0.0f;
}

void UPW_MaterialEffectComponent::BeginPlay()
{
	Super::BeginPlay();

	if (_autoGenerateMeshComponents)
	{
		GenerateMeshComponents();
	}

	SetMeshComponentsEffectValue(_startingEffectValue);
}

void UPW_MaterialEffectComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPW_MaterialEffectComponent, _currentEffectValue);
}

void UPW_MaterialEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	HandleEffect(DeltaTime);
}

void UPW_MaterialEffectComponent::GenerateMeshComponents()
{
	_skeletalMeshComponents = TArray<USkeletalMeshComponent*>();
	GetOwner()->GetComponents<USkeletalMeshComponent>(_skeletalMeshComponents);

	_staticMeshComponents = TArray<UStaticMeshComponent*>();
	GetOwner()->GetComponents<UStaticMeshComponent>(_staticMeshComponents);
}

void UPW_MaterialEffectComponent::HandleEffect(float DeltaTime)
{
	if (_isEffectActive)
	{
		if (_direction == EEffectDirection::ED_Forward && _currentEffectValue >= 1.0f || _direction == EEffectDirection::ED_Backward && _currentEffectValue <= 0.0f)
		{
			_isEffectActive = false;
			return;
		}
		
		_currentEffectValue = _direction == EEffectDirection::ED_Forward ? FMath::Clamp(_currentEffectValue + _fadeSpeed * DeltaTime, 0.0f, 1.0f) : FMath::Clamp(_currentEffectValue - _fadeSpeed * DeltaTime, 0.0f, 1.0f);
		SetMeshComponentsEffectValue(_currentEffectValue);
	}
}

void UPW_MaterialEffectComponent::SetMeshComponentsEffectValue(float value)
{
	for (USkeletalMeshComponent* meshComponent : _skeletalMeshComponents)
	{
		meshComponent->SetScalarParameterValueOnMaterials(_effectName, value);
	}

	for (UStaticMeshComponent* meshComponent : _staticMeshComponents)
	{
		meshComponent->SetScalarParameterValueOnMaterials(_effectName, value);
	}
}

void UPW_MaterialEffectComponent::OnRep_EffectValueChanged()
{
	SetMeshComponentsEffectValue(_currentEffectValue);
}

void UPW_MaterialEffectComponent::ActivateEffect(EEffectDirection direction)
{
	_direction = direction;
	_currentEffectValue = direction == EEffectDirection::ED_Forward ? 0.0f : 1.0f;
	_isEffectActive = true;
}

