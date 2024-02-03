// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PW_ItemHandlerComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_ItemHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UPW_ItemHandlerComponent();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetOverlappingItem(class APW_Item* Item);
	
	UFUNCTION() 
	void DoPickUp();

	UFUNCTION() 
	void DoSwitchItem();

	UFUNCTION() 
	void DoDrop();
	
	void EquipItem(APW_Item* Apw_Item);
	void UnEquipItem(APW_Item* Apw_Item);
	void DropItem();

	int _currentItemIndex = 0;
	
	FORCEINLINE APW_Item* GetOverlappingItem() const { return _overlappingItem; }
	FORCEINLINE APW_Item* GetItemInHand() const { return _itemInHand; }
	FORCEINLINE void SetItemHolder(USceneComponent* itemHolder) { _itemHolder = itemHolder; }

private:

	UPROPERTY(VisibleAnywhere, Category = "Character")
	USceneComponent* _itemHolder;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappinItem)
	class APW_Item* _overlappingItem;
	
	UPROPERTY(ReplicatedUsing = OnRep_ItemChange)
	class APW_Item* _itemInHand;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	TArray<APW_Item*> _itemsInInventory;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Handler")
	class APW_Character* _ownerCharacter;

	UFUNCTION()
	void OnRep_ItemChange(APW_Item* lastItem);
	
	UFUNCTION()
	void OnRep_OverlappinItem(APW_Item* lastItem);

	UFUNCTION(Server, Reliable)
	void ServerPickUp();

	UFUNCTION(Server, Reliable)
	void ServerEquip();

	UFUNCTION(Server, Reliable)
	void ServerDrop();
};
