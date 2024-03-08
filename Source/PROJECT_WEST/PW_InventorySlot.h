// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW_ItemObject.h"
#include "Engine/DataTable.h"
#include "PW_InventorySlot.generated.h"

enum class EItemType : uint8;
class APW_ItemObject;

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Inventory")
	EItemType _slotType;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	APW_ItemObject* _currentItem;

	FInventorySlot()
	{
		_currentItem = nullptr;
		_slotType = EItemType::EMeleeWeapon;
	}

	FInventorySlot(EItemType slotType)
	{
		_currentItem = nullptr;
		_slotType = slotType;		
	}

public:
	
	FORCEINLINE APW_ItemObject* GetItem() const { return _currentItem; }
	FORCEINLINE bool IsSlotAvailable() const { return _currentItem == nullptr; }
	FORCEINLINE void SetItem(APW_ItemObject* item) { _currentItem = item; }
	FORCEINLINE void RemoveItem() { _currentItem = nullptr; }
	FORCEINLINE EItemType GetSlotType() const { return _slotType; }
	FORCEINLINE void SetSlotType(EItemType type) { _slotType = type; }
};

USTRUCT(BlueprintType)
struct FItems: public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere,BlueprintReadOnly, Category = Data)
	FString _id;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class APW_ItemObject> _itemClass;
};

USTRUCT(BlueprintType)
struct FPlayerInventoryDataEntry
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere,BlueprintReadOnly, Category = Data)
	FString _playerName;

	UPROPERTY( EditAnywhere,BlueprintReadOnly, Category = Data)
	int32 _selectedSlotIndex;
	
	UPROPERTY( EditAnywhere,BlueprintReadOnly,Category = Data)
	TArray<FString> _itemIDs;
};

USTRUCT(BlueprintType)
struct FPlayersInventoryData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Data)
	TArray<FPlayerInventoryDataEntry> _playerInventorys;
	
	void AddInventory(const FString& playerName, const TArray<FString>& inventoryItemIDs, int32 selectedSlotIndex)
	{
		FPlayerInventoryDataEntry newEntry;
		newEntry._playerName = playerName;
		newEntry._itemIDs = inventoryItemIDs;
		newEntry._selectedSlotIndex = selectedSlotIndex;
		_playerInventorys.Add(newEntry);
	}

	FPlayerInventoryDataEntry GetInventoryData(const FString& playerName)
	{
		for (int i = 0; i < _playerInventorys.Num(); i++)
		{
			if (_playerInventorys[i]._playerName == playerName)
			{
				return _playerInventorys[i];
			}
		}
		return FPlayerInventoryDataEntry();
	}
	
	TArray<FString> GetInventoryItemIDs(FString playerName)
	{
		for (int i = 0; i < _playerInventorys.Num(); i++)
		{
			if (_playerInventorys[i]._playerName == playerName)
			{
				return _playerInventorys[i]._itemIDs;
			}
		}
		return TArray<FString>();
	}

	void Reset()
	{
		_playerInventorys.Empty();
	}

};