// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DebugMacros.h"
#include "GameFramework/Actor.h"
#include "Interfaces/PW_InteractableInterface.h"
#include "PW_ItemObject.generated.h"

class APW_Character;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputBindingDelegate, APW_Character*, characterOwner);

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
	
	UPROPERTY(EditAnywhere, Category = "Item Object")
	FString _itemID;
	
	UPROPERTY(EditAnywhere, Category = "Item Object")
	EItemType _itemType;

	UPROPERTY(EditAnywhere, Category = "Item Object")
	UStaticMeshComponent* _itemCollisionMesh;

	UPROPERTY(EditAnywhere, Category = "Item Object")
	USkeletalMeshComponent* _itemMesh;
	
	UPROPERTY(ReplicatedUsing = OnRep_ItemStateChanged, EditAnywhere, Category = "Item Object")
	EItemObjectState _itemState;

	UPROPERTY(ReplicatedUsing = OnRep_VisibilityChange, VisibleAnywhere, Category = "Item Properties")
	bool _isVisible;

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Item Object")
	bool _isActive;

	UPROPERTY(VisibleAnywhere, Category = "Item Object") 
	class UPW_HighlightCompont* _highlightComponent;

public:
	FInputBindingDelegate OnApplyInputBindingDelegate;
	FInputBindingDelegate OnRemoveInputBindingDelegate;

protected:
	
	virtual void BeginPlay() override;
	virtual void EnterHeldState();
	virtual void EnterDroppedState();

	UFUNCTION()
	virtual void OnRep_ItemStateChanged();	

	UFUNCTION()
	virtual void OnRep_VisibilityChange();

public:
	
	APW_ItemObject();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnUpdateItemState();
	virtual void OnSetVisibility();

	void ApplyActionBindings(APW_Character* characterOwner);
	virtual void LocalApplyActionBindings(APW_Character* characterOwner);
	UFUNCTION(Client, Reliable) void ClientApplyActionBindings(APW_Character* characterOwner);

	void RemoveActionBindings(APW_Character* characterOwner);
	virtual void LocalRemoveActionBindings(APW_Character* characterOwner);
	UFUNCTION(Client, Reliable) void ClientRemoveActionBindings(APW_Character* characterOwner);

	void SetVisibility(bool isVisible);
	void UpdateItemType(EItemType updatedType);
	void EnableItem(APW_Character* characterOwner);
	void DisableItem(APW_Character* characterOwner);
	void UpdateItemState(EItemObjectState updatedState);
	void AttemptAttachToOwner();

	virtual void StartFocus_Implementation() override;
	virtual void EndFocus_Implementation() override;
	virtual void StartInteract_Implementation(AActor* owner) override;

	FORCEINLINE FString GetItemID() const { return _itemID; }
	FORCEINLINE EItemType GetItemType() const { return _itemType; }
	FORCEINLINE EItemObjectState GetItemState() const { return _itemState; }
	FORCEINLINE UStaticMeshComponent* GetItemMesh() const { return _itemCollisionMesh; }
	FORCEINLINE void SetIsActive(bool isActive) { _isActive = isActive; }
	FORCEINLINE bool GetIsActive() const { return _isActive; }
};
