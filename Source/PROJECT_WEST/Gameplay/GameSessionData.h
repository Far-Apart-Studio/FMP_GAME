#pragma once

#include "CoreMinimal.h"
#include "PROJECT_WEST/Bounty System/PW_BountyData.h"
#include "GameSessionData.generated.h"

USTRUCT(BlueprintType)
struct FGameSessionData
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere,BlueprintReadOnly, Category = "Bounty System")
	TArray<FBountyDataEntry> _bountyDataList;
	
	UPROPERTY( EditAnywhere,BlueprintReadOnly, Category = "Bounty System")
	FBountyDataEntry _bountyDataEntry;

	UPROPERTY( EditAnywhere,BlueprintReadOnly, Category = "Bounty System")
	int32 _money;

	UPROPERTY( EditAnywhere, BlueprintReadOnly,Category = "Bounty System")
	bool _hasHead;

	UPROPERTY( EditAnywhere, BlueprintReadOnly,Category = "Bounty System")
	int32 _dayIndex = 1;
};
