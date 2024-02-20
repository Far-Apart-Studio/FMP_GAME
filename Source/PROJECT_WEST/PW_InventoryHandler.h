// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW_InventorySlot.h"
#include "PW_WeaponObject.h"
#include "Components/ActorComponent.h"
#include "PW_InventoryHandler.generated.h"

class APW_ItemObject;
class UPW_InventorySlot;
class APW_Character;
class APW_WeaponObject;
struct FInventorySlot;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_InventoryHandler : public UActorComponent
{
	GENERATED_BODY()

private:

	//DEBUG THINGS >>
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<APW_ItemObject> _spawnItemClass;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<APW_WeaponObject> _spawnWeaponClass;
	//DEBUG THINGS <<
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Inventory")
	APW_Character* _ownerCharacter;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<FInventorySlot> _slotConfiguration;

	UPROPERTY(Transient, VisibleAnywhere, Category = "Inventory")
	TArray<UPW_InventorySlot*> _inventorySlots;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	int _currentSlotIndex = 0;

public:	
	UPW_InventoryHandler();

private:
	void AssignInputActions();
	void GetOwnerCharacter();
	void CreateInventoryConfiguration();

protected:
	virtual void BeginPlay() override;

public:
	void ChangeSlot(const UPW_InventorySlot* updatedSlot, bool forceChangeSlot = false);
	void DropCurrentItem();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	bool TryGetSlot(int slotIndex, UPW_InventorySlot*& outSlot);
	bool TryCollectItem(APW_ItemObject* collectedItem);
	bool TryDropItem(UPW_InventorySlot* currentSlot);
	
	void ShowItem(APW_ItemObject* selectedItem);
	void HideItem(APW_ItemObject* selectedItem);

	bool TryGetAvailableSlot(EItemType itemType, UPW_InventorySlot*& outSlot);

	FORCEINLINE UPW_InventorySlot* GetSlot(int slotIndex) { return _inventorySlots[slotIndex]; }
	FORCEINLINE UPW_InventorySlot* GetCurrentSlot() { return GetSlot(_currentSlotIndex); }
	FORCEINLINE void CycleNextSlot(); 
	FORCEINLINE void CyclePreviousSlot();
	
	UFUNCTION() void CycleSlot();
};
