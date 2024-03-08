// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW_InventorySlot.h"
#include "PW_WeaponObject.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "PW_InventoryHandler.generated.h"

class APW_ItemObject;
class UPW_InventorySlot;
class APW_Character;
class APW_WeaponObject;
struct FInventorySlot;
struct FPlayerInventoryDataEntry;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_InventoryHandler : public UActorComponent
{
	GENERATED_BODY()

private:
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Inventory",meta = (AllowPrivateAccess = "true"))
	APW_Character* _ownerCharacter;

	UPROPERTY(EditAnywhere, Category = "Inventory",meta = (AllowPrivateAccess = "true"))
	TArray<EItemType> _defaultSlotTypes;
	
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Inventory",meta = (AllowPrivateAccess = "true"))
	TArray<FInventorySlot> _inventorySlots;

	UPROPERTY(EditAnywhere, Category = "Inventory",meta = (AllowPrivateAccess = "true"))
	float _throwVelocityMultiplier = 5.0f;

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Inventory",meta = (AllowPrivateAccess = "true"))
	int _currentSlotIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UDataTable* _ItemDataTable;

private:
	void AssignInputActions();
	void GetOwnerCharacter();
	TSubclassOf<class APW_ItemObject> GetItemObjectFromDataTable(FString id);
	
protected:
	
	virtual void BeginPlay() override;

public:
	UPW_InventoryHandler();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void LoadDefaultSlots();
	UFUNCTION(Server, Reliable)
	void ServerLoadDefaultSlotsByID();
	void LocalLoadDefaultSlotsByID();
	
	void CollectItem(APW_ItemObject* collectedItem);
	void LocalCollectItem(int slotIndex, APW_ItemObject* collectedItem);
	UFUNCTION(Server, Reliable) void ServerCollectItem(int slotIndex, APW_ItemObject* collectedItem);

	void DropItem(int slotIndex);
	void LocalDropItem(int slotIndex);
	UFUNCTION(Server, Reliable) void ServerDropItem(int slotIndex);
	
	void ChangeSlot(int targetedSlotIndex, bool forceChangeSlot = false);
	UFUNCTION(Server, Reliable)
	void SeverChangeSlot(int targetedSlotIndex);
	void LocalChangeSlot(int targetedSlotIndex);
	
	bool IsSlotValid(int slotIndex);
	bool TryGetSlotIndex(EItemType itemType, int& outIndex);

	void CyclePreviousSlot();

	void LoadItemsFromData (const FPlayerInventoryDataEntry& inventoryData);
	UFUNCTION( Server, Reliable )
	void ServerLoadFromData (const FPlayerInventoryDataEntry& inventoryData);
	void LocalLoadFromData (const FPlayerInventoryDataEntry& inventoryData);
	void CollectItems (const int32 selectedIndex, const TArray<APW_ItemObject*>& items);
	
	void AttachAllItems();

	void DropAllItems();
	UFUNCTION( Server, Reliable )
	void ServerDropAllItems();
	void LocalDropAllItems();

	TArray<FString> GetInventoryItemIDs();

	FORCEINLINE FInventorySlot& GetSlot(int slotIndex) { return _inventorySlots[slotIndex]; }
	FORCEINLINE FInventorySlot& GetCurrentSlot() { return GetSlot(_currentSlotIndex); }
	FORCEINLINE TArray<FInventorySlot>& GetInventorySlots() { return _inventorySlots; }

	UFUNCTION(BlueprintCallable) void CycleNextSlot(); 
	UFUNCTION(BlueprintCallable) void CycleUp();
	UFUNCTION(BlueprintCallable) void CycleDown();
	UFUNCTION(BlueprintCallable) void ToSlot(int targetedSlotIndex);
	UFUNCTION(BlueprintCallable) void DropCurrentItem();
	FORCENOINLINE int GetCurrentSlotIndex() const { return _currentSlotIndex; }
};
