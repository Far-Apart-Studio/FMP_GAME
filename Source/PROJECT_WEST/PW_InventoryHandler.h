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
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void CollectItem(APW_ItemObject* collectedItem);
	void LocalCollectItem(APW_ItemObject* collectedItem);
	UFUNCTION(Server, Reliable) void ServerCollectItem(APW_ItemObject* collectedItem);

	void DropItem(UPW_InventorySlot* inventorySlot);
	void LocalDropCurrentItem();
	UFUNCTION(Server, Reliable) void ServerDropCurrentItem();
	
	bool TryGetSlot(int slotIndex, UPW_InventorySlot*& outSlot);
	bool TryGetAvailableSlot(EItemType itemType, UPW_InventorySlot*& outSlot);
	void ChangeSlot(const UPW_InventorySlot* updatedSlot, bool forceChangeSlot = false);
	void EnableItem(APW_ItemObject* inventoryItem);
	void DisableItem(APW_ItemObject* selectedItem);
	void CycleNextSlot(); 
	void CyclePreviousSlot();

	FORCEINLINE UPW_InventorySlot* GetSlot(int slotIndex) { return _inventorySlots[slotIndex]; }
	FORCEINLINE UPW_InventorySlot* GetCurrentSlot() { return GetSlot(_currentSlotIndex); }
	
	UFUNCTION(BlueprintCallable) void CycleUp();
	UFUNCTION(BlueprintCallable) void CycleDown();
	UFUNCTION(BlueprintCallable) void ToSlot(int slotIndex);
	UFUNCTION(BlueprintCallable) void DropCurrentItem();
	UFUNCTION(BlueprintCallable) void DropAll();
};
