#pragma once

#include "CoreMinimal.h"
#include "PROJECT_WEST/Bounty System/PW_BountyData.h"
#include "PROJECT_WEST/PW_InventorySlot.h"
#include "GameSessionData.generated.h"

USTRUCT(BlueprintType)
struct FPlayerVisualDataEntry
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere,BlueprintReadOnly, Category = Data)
	FString _playerName;
	
	UPROPERTY( EditAnywhere,BlueprintReadOnly, Category = Data)
	int32 _selectedColorIndex;
};

USTRUCT(BlueprintType)
struct FPlayerVisualData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Data)
	TArray<FPlayerVisualDataEntry> _playerVisuals;
	
	void AddData(const FString& playerName, int32 _selectedColorIndex)
	{
		FPlayerVisualDataEntry newEntry;
		newEntry._playerName = playerName;
		newEntry._selectedColorIndex = _selectedColorIndex;
		_playerVisuals.Add(newEntry);
	}
	
	FPlayerVisualDataEntry GetVisualData(const FString& playerName)
	{
		for (int i = 0; i < _playerVisuals.Num(); i++)
		{
			if (_playerVisuals[i]._playerName == playerName)
			{
				return _playerVisuals[i];
			}
		}
		return FPlayerVisualDataEntry();
	}
	
	void Reset()
	{
		_playerVisuals.Empty();
	}
};

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

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data)
	TArray<FString> _escapedPlayers;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data)
	FPlayerVisualData _playersVisualData;
};