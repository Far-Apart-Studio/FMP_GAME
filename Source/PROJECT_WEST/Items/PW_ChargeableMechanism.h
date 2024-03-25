// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PROJECT_WEST/Interfaces//PW_InteractableInterface.h"
#include "PW_ChargeableMechanism.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActivatedDelegate, APW_ChargeableMechanism*, chargeableMechanism);
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
	
	virtual void StartFocus_Implementation(AActor* owner) override;
	virtual void EndFocus_Implementation(AActor* owner) override;
	virtual void StartInteract_Implementation(AActor* owner) override;
	virtual void EndInteract_Implementation() override;
	virtual bool IsInteracting_Implementation() override;

	bool IsLanternEquipped(AActor* interactingActor);

	UPROPERTY(BlueprintAssignable)
	FOnFocuseBeginDelegate _OnFocusBegin;

	UPROPERTY(BlueprintAssignable)
	FOnFocusEndDelegate _OnFocusEnd;

	UPROPERTY(BlueprintAssignable)
	FOnActivatedDelegate _OnActivated;

private:

	class APW_Lantern* _lantern;

	bool _chargeActivated = false;

	void HandleCharging(float DeltaTime);
};
