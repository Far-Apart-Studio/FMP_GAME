// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PROJECT_WEST/Interfaces//PW_InteractableInterface.h"
#include "PW_ChargeableMechanism.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChargeCompletedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChargeAmountDelegate, float, normalisedValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFocuseBeginDelegate, bool , hasLanterEquipped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE (FOnFocusEndDelegate);

UCLASS()
class PROJECT_WEST_API APW_ChargeableMechanism : public AActor, public IPW_InteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APW_ChargeableMechanism();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void StartFocus_Implementation(AActor* targetActor) override;
	virtual void EndFocus_Implementation(AActor* targetActor) override;

	virtual bool HasServerInteraction_Implementation() override;
	virtual void ServerStartInteract_Implementation(AActor* targetActor) override;
	virtual void ServerStopInteract_Implementation() override;
	
	virtual void StartInteract_Implementation(AActor* owner) override;
	virtual void EndInteract_Implementation() override;
	
	virtual bool IsInteracting_Implementation() override;

	UFUNCTION()
	void OnChargeButtonHeld(bool bValue);

	bool IsLanternEquipped(AActor* interactingActor);

	UPROPERTY(BlueprintAssignable)
	FOnFocuseBeginDelegate _OnFocusBegin;

	UPROPERTY(BlueprintAssignable)
	FOnFocusEndDelegate _OnFocusEnd;

	UPROPERTY(BlueprintAssignable)
	FOnChargeCompletedDelegate _OnChargeCompleted;

	UPROPERTY(BlueprintAssignable)
	FOnChargeAmountDelegate _OnChargeAmountChanged;

private:
	
	UPROPERTY(Replicated,VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess = "true"))
	class APW_Character* _character;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess = "true"))
	class APW_Lantern* _lantern;

	UPROPERTY(Replicated,VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess = "true"))
	bool _chargeActivated = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess = "true"))
	float _maxChargeAmount = 100.0f;

	UPROPERTY(ReplicatedUsing= OnRep_ChargeAmountChanged, VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess = "true"))
	float _currentChargeAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess = "true"))
	float _chargeRate = 10.0f;

	UPROPERTY(ReplicatedUsing= OnRep_ChargeStatusChanged, VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess = "true"))
	bool _chargeCompleted;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess = "true"))
	class UPW_HighlightCompont* _highlightComponent;

	void ForceEndInteraction() const;

	UFUNCTION()
	void OnRep_ChargeAmountChanged() const;

	UFUNCTION()
	void OnRep_ChargeStatusChanged() const;

	void HandleCharging(float DeltaTime);

	void ToggleCharacterMovement(bool bValue) const;

	bool IsFullyCharged() const;
};
