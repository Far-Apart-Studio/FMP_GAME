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

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TArray<UPW_InventorySlot*> _inventorySlots;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	int _currentSlotIndex = 0;

private:
	void AssignInputActions();
	void GetOwnerCharacter();
	void CreateInventoryConfiguration();

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
	
	void ChangeSlot(const UPW_InventorySlot* targetedSlot, bool forceChangeSlot = false);
	bool TryGetSlot(int slotIndex, UPW_InventorySlot*& outSlot);
	bool TryGetAvailableSlot(EItemType itemType, UPW_InventorySlot*& outSlot);
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
