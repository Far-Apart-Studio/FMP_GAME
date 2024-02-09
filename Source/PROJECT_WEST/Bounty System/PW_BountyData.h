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

USTRUCT(BlueprintType)
struct FBountyDifficultyDataEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounty System")
	EBountyDifficulty _bountyDifficulty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounty System")
	int32 _bountyMinCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounty System")
	int32 _bountyMaxCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounty System")
	int32 _bountyMinReward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounty System")
	int32 _bountyMaxReward;
};

USTRUCT(BlueprintType)
struct FBountyDifficultyData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounty System")
	TArray<FBountyDifficultyDataEntry> _bountyDifficultyData;

	int32 GetBountyCost(EBountyDifficulty bountyDifficulty)
	{
		for (FBountyDifficultyDataEntry& entry : _bountyDifficultyData)
		{
			if (entry._bountyDifficulty == bountyDifficulty)
			{
				return FMath::RandRange(entry._bountyMinCost, entry._bountyMaxCost);
			}
		}
		return	0;
	}

	int32 GetBountyReward(EBountyDifficulty bountyDifficulty)
	{
		for (FBountyDifficultyDataEntry& entry : _bountyDifficultyData)
		{
			if (entry._bountyDifficulty == bountyDifficulty)
			{
				return FMath::RandRange(entry._bountyMinReward, entry._bountyMaxReward);
			}
		}
		return	0;
	}
};

USTRUCT(BlueprintType)
struct FBountyMapDataEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Bounty Data")
	UTexture2D* _bountyMapIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounty System")
	FString _bountyMapName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounty System")
	FString _bountyMapDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounty System")
	FString _bountyMapPath;
};

USTRUCT(BlueprintType)
struct FBountyMapData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounty System")
	TArray<FBountyMapDataEntry> _bountyMapData;

	FBountyMapDataEntry GetRandomBountyMapDataEntry()
	{
		int32 randomIndex = FMath::RandRange(0, _bountyMapData.Num() - 1);
		return _bountyMapData[randomIndex];
	}
};


USTRUCT(BlueprintType)
struct FBountyTargetEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounty System")
	FString _bountyTargetName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounty System")
	FString _bountyTargetDescription;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounty System")
	TSubclassOf<AActor> _bountyTarget;
};

USTRUCT(BlueprintType)
struct FBountyTargets
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounty System")
	TArray<FBountyTargetEntry> _bountyTargets;

	FBountyTargetEntry GetRandomBountyTarget()
	{
		int32 randomIndex = FMath::RandRange(0, _bountyTargets.Num() - 1);
		return _bountyTargets[randomIndex];
	}
};

USTRUCT(BlueprintType)
struct FBountyDataEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounty System")
	EBountyDifficulty _bountyDifficulty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounty System")
	int32 _bountyCost;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounty System")
	int32 _bountyReward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounty System")
	FBountyMapDataEntry _bountyMapDataEntry;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounty System")
	FBountyTargetEntry _bountyTarget;
};