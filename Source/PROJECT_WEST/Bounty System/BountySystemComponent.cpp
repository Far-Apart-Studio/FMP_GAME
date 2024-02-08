// Fill out your copyright notice in the Description page of Project Settings.


#include "BountySystemComponent.h"

#include "PROJECT_WEST/DebugMacros.h"

// Sets default values for this component's properties
UBountySystemComponent::UBountySystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBountySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UBountySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FBountyDataEntry UBountySystemComponent::GetBountyDataEntry(EBountyDifficulty _bountyDifficulty)
{
	FBountyDataEntry bountyDataEntry = FBountyDataEntry();
	bountyDataEntry._bountyDifficulty = _bountyDifficulty;
	bountyDataEntry._bountyCost = 100;
	bountyDataEntry._bountyReward = _bountyDifficultyData.GetBountyReward(_bountyDifficulty);
	bountyDataEntry._bountyMapDataEntry = _bountyMapData.GetRandomBountyMapDataEntry();
	bountyDataEntry._bountyTarget = _bountyTargets.GetRandomBountyTarget();
	return bountyDataEntry;
}

TArray<FBountyDataEntry> UBountySystemComponent::GetBountyDataList(TArray<EBountyDifficulty> _bountyDifficulties)
{
	TArray<FBountyDataEntry> bountyDataList;
	for (EBountyDifficulty difficulty : _bountyDifficulties)
	{
		bountyDataList.Add(GetBountyDataEntry(difficulty));
	}
	return bountyDataList;
}
