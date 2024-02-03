// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PW_Item.generated.h"

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Init UMETA(DisplayName = "Init"),
	EIS_Pickup UMETA(DisplayName = "Pickup"),
	EIS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class PROJECT_WEST_API APW_Item : public AActor
{
	GENERATED_BODY()
	
public:	

	APW_Item();

protected:

	virtual void BeginPlay() override;

	virtual void OnRep_Owner() override;
	virtual void OnItemStateSet();
	virtual void OnPicked();
	virtual void OnDropped();

	
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex);

	UPROPERTY()
	class APW_Character* _ownerCharacter;
	
	UPROPERTY()
	class APW_PlayerController* _ownerPlayerController;

	UFUNCTION()
	void OnPingTooHigh(bool bPingTooHigh);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void ShowPickupWidget(bool bShowWidget);
	void EnableCustomDepth(bool bEnable);
	virtual void OnVisibilityChange(bool bIsVisible);
	void SetItemState(EItemState State);
	
	FORCEINLINE class USphereComponent* GetAreaSphere() const { return _areaSphere; }
	//FORCEINLINE class USkeletalMeshComponent* GetWeaponMesh() const { return _itemMesh; }
	FORCEINLINE class UWidgetComponent* GetPickupWidget() const { return _pickupWidget; }
	FORCEINLINE void SetVisibility(bool bIsVisible) { _isVisible = bIsVisible; }

protected:
	UPROPERTY(VisibleAnywhere, Category = "Item Properties")
	UStaticMeshComponent* _itemMesh;
	
	UPROPERTY(VisibleAnywhere, Category = "Item Properties")
	class USphereComponent* _areaSphere;

	UPROPERTY(ReplicatedUsing = OnRep_ItemState, VisibleAnywhere, Category = "Item Properties")
	EItemState _itemState;

	UPROPERTY(ReplicatedUsing = OnRep_VisibilityChange, VisibleAnywhere, Category = "Item Properties")
	bool _isVisible;

	UFUNCTION()
	void OnRep_ItemState();

	UFUNCTION()
	void OnRep_VisibilityChange();

	UPROPERTY(Replicated, EditAnywhere)
	bool _bUseServerSideRewind = false;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* _pickupWidget;
};
