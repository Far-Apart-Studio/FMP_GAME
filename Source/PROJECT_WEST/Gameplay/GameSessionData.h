#pragma once

#include "CoreMinimal.h"
#include "PROJECT_WEST/Bounty System/PW_BountyData.h"
#include "PROJECT_WEST/PW_InventorySlot.h"
#include "GameSessionData.generated.h"

USTRUCT(BlueprintType)
struct FGameSessionData
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data)
	TArray<FBountyDataEntry> _bountyDataList;
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data)
	FBountyDataEntry _bountyDataEntry;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data)
	int32 _money;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data)
	bool _hasHead;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data)
	int32 _dayIndex;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data)
	FPlayersInventoryData _playersInventoryData;
};