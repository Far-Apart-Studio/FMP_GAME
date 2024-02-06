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
	void TryPickUpItemNearBy();

	UFUNCTION() 
	void DoSwitchItem();

	void DoPickUp( APW_Item* item );
	void DoEquip( APW_Item* item );
	void DoDrop( APW_Item* item );
	void DoDropAllItems();

	UFUNCTION() 
	void TryDropItemHeld();

	void PickUpItem(APW_Item* item);
	void EquipItem(APW_Item* item);
	void UnEquipItem(APW_Item* item);
	
	void DropItem(APW_Item* item);
	void DropAllItems();

	template<class T>
	bool HasItemType ();
	
	FORCEINLINE APW_Item* GetOverlappingItem() const { return _overlappingItem; }
	FORCEINLINE APW_Item* GetItemInHand() const { return _itemInHand; }
	FORCEINLINE void SetOwnerCharacter(class APW_Character* ownerCharacter) { _ownerCharacter = ownerCharacter; }
private:

	UPROPERTY(ReplicatedUsing = OnRep_OverlappinItem, VisibleAnywhere, Category = "Item")
	class APW_Item* _overlappingItem;
	
	UPROPERTY(Replicated, VisibleAnywhere,BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	class APW_Item* _itemInHand;

	UPROPERTY(Replicated,  VisibleAnywhere, Category = "Item")
	TArray<APW_Item*> _itemsInInventory;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Handler")
	class APW_Character* _ownerCharacter;
	
	UFUNCTION()
	void OnRep_OverlappinItem(APW_Item* lastItem);

	UFUNCTION(Server, Reliable)
	void ServerPickUp(APW_Item* Item);

	UFUNCTION(Server, Reliable)
	void ServerEquip(APW_Item* item);

	UFUNCTION(Server, Reliable)
	void ServerUnEquip(APW_Item* item);

	UFUNCTION(Server, Reliable)
	void ServerDrop(APW_Item* item);

	UFUNCTION(Server, Reliable)
	void ServerDropAllItems();
};

template <class T>
bool UPW_ItemHandlerComponent::HasItemType()
{
	for (APW_Item* itemInInventory : _itemsInInventory)
	{
		if (itemInInventory->IsA(T::StaticClass()))
		{
			return true;
		}
	}
	return false;
}
