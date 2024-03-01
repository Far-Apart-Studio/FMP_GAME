// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DebugMacros.h"
#include "GameFramework/Actor.h"
#include "Interfaces/PW_InteractableInterface.h"
#include "PW_ItemObject.generated.h"

class APW_Character;

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
	EHeld,
	EDropped
};

UCLASS()
class PROJECT_WEST_API APW_ItemObject : public AActor, public IPW_InteractableInterface
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Item Object")
	EItemType _itemType;

	UPROPERTY(Replicated, EditAnywhere, Category = "Item Object")
	UStaticMeshComponent* _itemMesh;
	
	UPROPERTY(Replicated, EditAnywhere, Category = "Item Object")
	EItemObjectState _itemState;

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Item Object")
	bool _isActive;

protected:
	virtual void BeginPlay() override;
	virtual void EnterHeldState();
	virtual void EnterDroppedState();

public:
	APW_ItemObject();
	
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void BindActions(APW_Character* characterOwner);
	virtual void LocalBindActions(APW_Character* characterOwner);
	UFUNCTION(Client, Reliable) void ClientBindActions(APW_Character* characterOwner);
	
	virtual void RemoveBindingActions(APW_Character* characterOwner) {}
	
	void UpdateItemState(EItemObjectState updatedState);
	void UpdateItemType(EItemType updatedType);
	void SetVisibility(bool isVisible);

	virtual void StartFocus_Implementation() override;
	virtual void EndFocus_Implementation() override;
	virtual void StartInteract_Implementation(AActor* owner) override;

	FORCEINLINE EItemType GetItemType() const { return _itemType; }
	FORCEINLINE EItemObjectState GetItemState() const { return _itemState; }
	FORCEINLINE UStaticMeshComponent* GetItemMesh() const { return _itemMesh; }
	FORCEINLINE void SetIsActive(bool isActive) { _isActive = isActive; }
	FORCEINLINE bool GetIsActive() const { return _isActive; }
};
