// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW_InventorySlot.h"
#include "PW_ItemObject.h"
#include "Components/ActorComponent.h"
#include "Items/PW_Item.h"
#include "PW_InventoryHandler.generated.h"

class APW_ItemObject;
class UPW_InventorySlot;
class APW_Character;
struct FInventorySlot;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_InventoryHandler : public UActorComponent
{
	GENERATED_BODY()

private:

	//DEBUG THINGS >>

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<APW_ItemObject> _spawnItemClass;
	
	//DEBUG THINGS <<
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Inventory")
	APW_Character* _ownerCharacter;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<FInventorySlot> _currentSlots;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<UPW_InventorySlot*> _inventorySlots;
	
	int _currentSlotIndex = 0;

public:	
	UPW_InventoryHandler();


private:
	void AssignInputActions();
	void GetOwnerCharacter();

protected:
	virtual void BeginPlay() override;

public:
	void ChangeSlot(const FInventorySlot& updatedSlot);
	void DropCurrentItem();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	bool TryGetSlot(int slotIndex, FInventorySlot& outSlot);
	bool TryCollectItem(APW_ItemObject* collectedItem);
	bool TryDropItem(FInventorySlot& currentSlot);
	
	void ShowItem(APW_ItemObject* selectedItem);
	void HideItem(APW_ItemObject* selectedItem);

	FInventorySlot TryGetAvailableSlot(EItemType itemType);

	FORCEINLINE FInventorySlot GetSlot(int slotIndex) { return _currentSlots[slotIndex]; }
	FORCEINLINE FInventorySlot GetCurrentSlot() { return GetSlot(_currentSlotIndex); }
	FORCEINLINE FInventorySlot GetNextSlot() { return GetSlot(_currentSlotIndex + 1); }
	FORCEINLINE FInventorySlot GetPreviousSlot() { return GetSlot(_currentSlotIndex - 1); }


	//DEBUG FUNCTIONS >>>
	void SpawnItem();
	//DEBUG FUNCTIONS <<<
};
