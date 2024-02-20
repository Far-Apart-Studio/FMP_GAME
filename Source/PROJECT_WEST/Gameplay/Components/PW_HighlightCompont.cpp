// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_HighlightCompont.h"


UPW_HighlightCompont::UPW_HighlightCompont()
{

	PrimaryComponentTick.bCanEverTick = true;
}

void UPW_HighlightCompont::BeginPlay()
{
	Super::BeginPlay();

	_meshComps = TArray<UStaticMeshComponent*>();
	GetOwner()->GetComponents<UStaticMeshComponent>(_meshComps);

	_skeletalMeshComps = TArray<USkeletalMeshComponent*>();
	GetOwner()->GetComponents<USkeletalMeshComponent>(_skeletalMeshComps);
}

void UPW_HighlightCompont::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPW_HighlightCompont::ShowHighlight()
{
	for (UStaticMeshComponent* meshComp : _meshComps)
	{
		meshComp->SetRenderCustomDepth(true);
	}

	for ( USkeletalMeshComponent* meshComp : _skeletalMeshComps)
	{
		meshComp->SetRenderCustomDepth(true);
	}
}

void UPW_HighlightCompont::HideHighlight()
{
	for (UStaticMeshComponent* meshComp : _meshComps)
	{
		meshComp->SetRenderCustomDepth(false);
	}

	for (USkeletalMeshComponent* meshComp : _skeletalMeshComps)
	{
		meshComp->SetRenderCustomDepth(false);
	}
}

