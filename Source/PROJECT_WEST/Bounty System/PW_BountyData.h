// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PW_BountyData.generated.h"

UENUM()
enum class EBountyDifficulty: uint8
{
	OneStar,
	TwoStar,
	ThreeStar,
	FourStar,
	FiveStar
};

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API UPW_BountyData : public UDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Bounty Data")
	UTexture2D* _bountyIcon;
	
	UPROPERTY(EditAnywhere, Category = "Bounty Data")
	FString _bountyName;

	UPROPERTY(EditAnywhere, Category = "Bounty Data")
	FString _bountyDescription;
};
