// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW_InventorySlot.generated.h"

enum class EItemType : uint8;
class APW_ItemObject;

USTRUCT()
struct FInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Inventory")
	EItemType SlotType;
};

UCLASS()
class PROJECT_WEST_API UPW_InventorySlot : public UObject
{
	GENERATED_BODY()

private:
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

