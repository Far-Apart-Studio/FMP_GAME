// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_HighlightComponent.h"


UPW_HighlightComponent::UPW_HighlightComponent()
{

	PrimaryComponentTick.bCanEverTick = true;
}

void UPW_HighlightComponent::BeginPlay()
{
	Super::BeginPlay();

	_meshComps = TArray<UMeshComponent*>();
	GetOwner()->GetComponents<UMeshComponent>(_meshComps);
}

void UPW_HighlightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPW_HighlightComponent::ShowHighlight()
{
	for (UMeshComponent* component : _meshComps)
	{
		if (_meshComponentsToIgnore.Contains(component))
		{
			continue;
		}
		component->SetRenderCustomDepth(true);
	}
}

void UPW_HighlightComponent::HideHighlight()
{
	for (UMeshComponent* component : _meshComps)
	{
		if (_meshComponentsToIgnore.Contains(component))
		{
			continue;
		}
		component->SetRenderCustomDepth(false);
	}
}

