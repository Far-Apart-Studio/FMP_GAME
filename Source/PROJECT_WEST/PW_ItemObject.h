// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DebugMacros.h"
#include "GameFramework/Actor.h"
#include "Interfaces/PW_InteractableInterface.h"
#include "PW_ItemObject.generated.h"

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

	UPROPERTY(EditAnywhere, Category = "Item Object")
	EItemObjectState _itemState;

	UPROPERTY(EditAnywhere, Category = "Item Object")
	UStaticMeshComponent* _itemMesh;

private:
	void EnterHeldState();
	void EnterDroppedState();

protected:
	virtual void BeginPlay() override;

public:
	APW_ItemObject();
	
	virtual void Tick(float DeltaTime) override;
	virtual void Use() { DEBUG_STRING("USE ITEM"); }

	void UpdateItemState(EItemObjectState updatedState);
	void UpdateItemType(EItemType updatedType);
	void SetVisibility(bool isVisible);

	virtual void StartFocus_Implementation() override;
	virtual void EndFocus_Implementation() override;
	virtual void StartInteract_Implementation(AActor* owner) override;
	virtual void EndInteract_Implementation() override;
	virtual bool IsInteracting_Implementation() override;

	FORCEINLINE EItemType GetItemType() const { return _itemType; }
	FORCEINLINE EItemObjectState GetItemState() const { return _itemState; }
	FORCEINLINE void SetItemState(EItemObjectState newState) { _itemState = newState; }
	FORCEINLINE UStaticMeshComponent* GetItemMesh() const { return _itemMesh; }
};
