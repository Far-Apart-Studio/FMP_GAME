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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_InventoryHandler : public UActorComponent
{
	GENERATED_BODY()

private:
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Inventory")
	APW_Character* _ownerCharacter;
	
	UPROPERTY(Replicated, EditAnywhere, Category = "Inventory")
	TArray<FInventorySlot> _inventorySlots;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float _throwVelocityMultiplier = 5.0f;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
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
	
	void CollectItem(APW_ItemObject* collectedItem);
	void LocalCollectItem(APW_ItemObject* collectedItem);
	UFUNCTION(Server, Reliable) void ServerCollectItem(APW_ItemObject* collectedItem);

	void DropItem(int slotIndex);
	void LocalDropItem(APW_ItemObject* slotItem);
	UFUNCTION(Server, Reliable) void ServerDropItem(APW_ItemObject* slotItem);
	
	void ChangeSlot(int targetedSlotIndex, bool forceChangeSlot = false);
	bool IsSlotValid(int slotIndex);
	bool TryGetSlotIndex(EItemType itemType, int& outIndex);
	void CycleNextSlot(); 
	void CyclePreviousSlot();
	
	void LoadItems (const TArray<APW_ItemObject*>& items);

	void LoadItemsByID (const TArray<FString>& itemIDs);
	UFUNCTION( Server, Reliable )
	void ServerLoadItems(const TArray<FString>& itemIDs);
	void LocalLoadItems(const TArray<FString>& itemIDs);

	TArray<FString> GetInventoryItemIDs();

	FORCEINLINE FInventorySlot& GetSlot(int slotIndex) { return _inventorySlots[slotIndex]; }
	FORCEINLINE FInventorySlot& GetCurrentSlot() { return GetSlot(_currentSlotIndex); }
	FORCEINLINE TArray<FInventorySlot>& GetInventorySlots() { return _inventorySlots; }
	
	UFUNCTION(BlueprintCallable) void CycleUp();
	UFUNCTION(BlueprintCallable) void CycleDown();
	UFUNCTION(BlueprintCallable) void ToSlot(int targetedSlotIndex);
	UFUNCTION(BlueprintCallable) void DropCurrentItem();
	UFUNCTION(BlueprintCallable) void DropAll();

};
