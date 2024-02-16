// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW_InventorySlot.generated.h"

enum class EItemType : uint8;
class APW_ItemObject;

UCLASS()
class PROJECT_WEST_API UPW_InventorySlot : public UObject
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	EItemType _slotType;
	
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TArray<APW_ItemObject*> _currentItems;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int _currentIndex = 0;

public:
	
	FORCEINLINE APW_ItemObject* GetCurrentItem() const { return _currentItems[_currentIndex]; }
	FORCEINLINE void AppendItem(APW_ItemObject* item) { _currentItems.Add(item); }
	FORCEINLINE void RemoveItem(APW_ItemObject* item) { _currentItems.Remove(item); }
	FORCEINLINE EItemType GetSlotType() const { return _slotType; }
	
	void EmptyCurrentSlot();
	bool IsSlotAvailable();
};
