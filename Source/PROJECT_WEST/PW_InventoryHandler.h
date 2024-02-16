// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW_ItemObject.h"
#include "Components/ActorComponent.h"
#include "Items/PW_Item.h"
#include "PW_InventoryHandler.generated.h"

class APW_ItemObject;
class UPW_InventorySlot;
class APW_Character;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_InventoryHandler : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Inventory")
	APW_Character* _ownerCharacter;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<UPW_InventorySlot*> _inventorySlots;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	APW_ItemObject* _currentSelectedItem;

	int _currentSlotIndex = 0;

public:	
	UPW_InventoryHandler();


private:
	void AssignInputActions();
	void AssignDefaultSlot();
	void GetOwnerCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	bool TryCollectItem(APW_ItemObject* collectedItem);
	bool TryDropItem(APW_ItemObject* droppedItem);
	void ShowItem(APW_ItemObject* selectedItem);
	void HideItem(APW_ItemObject* selectedItem);
	
	UPW_InventorySlot* GetAvailableSlot(EItemType itemType);

	FORCEINLINE UPW_InventorySlot* GetSlot(int slotIndex) { return _inventorySlots[slotIndex]; }
	FORCEINLINE UPW_InventorySlot* GetCurrentSlot() { return _inventorySlots[_currentSlotIndex]; }
	
	FORCEINLINE void DropHeldItem() { TryDropItem(_currentSelectedItem); }
	FORCEINLINE void UseHeldItem() { _currentSelectedItem->Use(); }
};
