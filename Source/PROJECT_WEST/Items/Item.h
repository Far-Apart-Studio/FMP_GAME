// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Initial UMETA(DisplayName = "Initial State"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class PROJECT_WEST_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	

	AItem();

protected:

	virtual void BeginPlay() override;
	virtual void OnItemStateSet();
	virtual void OnEquipped();
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
	virtual void OnRep_Owner() override;
	virtual void Dropped();
	void ShowPickupWidget(bool bShowWidget);
	void EnableCustomDepth(bool bEnable);

	void SetItemState(EItemState State);
	
	FORCEINLINE class USphereComponent* GetAreaSphere() const { return _areaSphere; }
	FORCEINLINE class USkeletalMeshComponent* GetWeaponMesh() const { return _itemMesh; }
	FORCEINLINE class UWidgetComponent* GetPickupWidget() const { return _pickupWidget; }

protected:
	UPROPERTY(VisibleAnywhere, Category = "Item Properties")
	USkeletalMeshComponent* _itemMesh;

	UPROPERTY(VisibleAnywhere, Category = "Item Properties")
	class USphereComponent* _areaSphere;

	UPROPERTY(ReplicatedUsing = OnRep_ItemState, VisibleAnywhere, Category = "Item Properties")
	EItemState _itemState;

	UFUNCTION()
	void OnRep_ItemState();

	UPROPERTY(Replicated, EditAnywhere)
	bool _bUseServerSideRewind = false;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* _pickupWidget;
	
};
