// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	
	UPROPERTY( EditAnywhere,BlueprintReadOnly,Category = Data)
	TArray<FString> _itemIDs;
};

USTRUCT(BlueprintType)
struct FPlayersInventoryData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = Data)
	TArray<FPlayerInventoryDataEntry> _playerInventorys;
	
	void AddInventory(FString playerName, const TArray<FString>& inventoryItemIDs)
	{
		RemoveInventory (playerName);
		
		FPlayerInventoryDataEntry newEntry;
		newEntry._playerName = playerName;
		newEntry._itemIDs.Empty();
		newEntry._itemIDs = inventoryItemIDs;
		_playerInventorys.Add(newEntry);
	}

	void RemoveInventory(FString playerName)
	{
		for (int i = 0; i < _playerInventorys.Num(); i++)
		{
			if (_playerInventorys[i]._playerName == playerName)
			{
				_playerInventorys.RemoveAt(i);
				break;
			}
		}
	}
};