// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/PW_InteractableInterface.h"
#include "InfoData.h"
#include "PW_ItemObject.generated.h"

class UPW_InventoryHandler;
class APW_Character;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemActionDelegate, APW_Character*, characterOwner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickedUp, APW_ItemObject*, itemPicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FItemStateDelegate);

UENUM(BlueprintType)
enum class EItemType : uint8
{
	ERangedWeapon,
	EMeleeWeapon,
	ESpecial,
	EUtility
};

UENUM(BlueprintType)
enum class EItemObjectState : uint8
{
	EDropped,
	EHeld,
};

UCLASS()
class PROJECT_WEST_API APW_ItemObject : public AActor, public IPW_InteractableInterface
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(EditAnywhere, Category = "Item Object",meta = (AllowPrivateAccess = "true"))
	FString _itemID;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Item Object",meta = (AllowPrivateAccess = "true"))
	FInfoEntry _infoDetails;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Item Object",meta = (AllowPrivateAccess = "true"))
	UTexture2D* _itemIcon;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Item Object", meta = (AllowPrivateAccess = "true"))
	EItemType _itemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Object")
	UStaticMeshComponent* _itemCollisionMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Object")
	USkeletalMeshComponent* _itemMesh;
	
	UPROPERTY(ReplicatedUsing = OnRep_ItemStateChanged, BlueprintReadOnly, EditAnywhere, Category = "Item Object")
	EItemObjectState _itemState;

	UPROPERTY(ReplicatedUsing = OnRep_VisibilityChange, VisibleAnywhere, Category = "Item Properties")
	bool _isVisible;

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Item Object")
	bool _isActive;

	UPROPERTY(VisibleAnywhere, Category = "Item Object") 
	class UPW_HighlightComponent* _highlightComponent;

public:

	/**
	 * This delegates are used to apply object actions. These can be used interaction with the "ApplyObjectActions" function.
	 * This also how you are intended to implement the functionality within blueprint.
	 */
	
	UPROPERTY(BlueprintAssignable) FOnItemActionDelegate OnApplyObjectActions;
	UPROPERTY(BlueprintAssignable) FOnItemActionDelegate OnClearObjectActions;
	UPROPERTY(BlueprintAssignable) FItemStateDelegate OnEnterDroppedState;
	UPROPERTY(BlueprintAssignable) FItemStateDelegate OnEnterHeldState;
	FOnPickedUp _onPickedUpServer;

protected:
	
	virtual void BeginPlay() override;
	virtual void EnterHeldState();
	virtual void EnterDroppedState();

	UFUNCTION() virtual void OnRep_ItemStateChanged();	
	UFUNCTION() virtual void OnRep_VisibilityChange();

public:
	
	APW_ItemObject();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnUpdateItemState();
	virtual void OnSetVisibility();
	
	/**
	* Executes actions upon equipping an item. Consider renaming to accurately reflect its purpose.
	* This function is primarily utilized for action binding upon item equipping, but it can be adapted
	* for various other functionalities when an item is equipped.
	* @param characterOwner The owning character associated with the equipped item.
	*/
	
	void ApplyActionBindings(APW_Character* characterOwner);
	
	virtual void ApplyObjectActions(APW_Character* characterOwner);
	
	UFUNCTION(Client, Reliable) void ClientApplyActionBindings(APW_Character* characterOwner);

	/**
	 * This function is the counterpart to ApplyActionBindings. It is primarily utilized for action unbinding
	 * @param characterOwner 
	 */
	
	void RemoveActionBindings(APW_Character* characterOwner);
	
	virtual void ClearObjectActions(APW_Character* characterOwner);
	
	UFUNCTION(Client, Reliable) void ClientRemoveActionBindings(APW_Character* characterOwner);
	
	void SetVisibility(bool isVisible);
	void UpdateItemType(EItemType updatedType);
	void EnableItem(APW_Character* characterOwner);
	void DisableItem(APW_Character* characterOwner);
	void UpdateItemState(EItemObjectState updatedState);
	void AttemptAttachToOwner();
	UPW_InventoryHandler* GetHolderInventory();

	virtual void StartFocus_Implementation(AActor* owner) override;
	virtual void EndFocus_Implementation(AActor* owner) override;
	virtual void StartInteract_Implementation(AActor* owner) override;


	FORCEINLINE FString GetItemID() const { return _itemID; }
	FORCEINLINE EItemType GetItemType() const { return _itemType; }
	FORCEINLINE EItemObjectState GetItemState() const { return _itemState; }
	FORCEINLINE void SetIsActive(bool isActive) { _isActive = isActive; }
	FORCEINLINE bool GetIsActive() const { return _isActive; }
	
	UFUNCTION(BlueprintCallable) UStaticMeshComponent* GetCollisionMesh() const { return _itemCollisionMesh; }
	UFUNCTION(BlueprintCallable) USkeletalMeshComponent* GetVisualMesh() const { return _itemMesh; }
};
