// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PW_InteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_InteractionComponent : public UActorComponent
{
	
	GENERATED_BODY()

public:	

	UPW_InteractionComponent();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void TryClearLastInteractable();
	void TraceForInteractable();

	UFUNCTION()
	void TryStartInteractWithInteractable();
	
	UFUNCTION()
	void TryEndInteractWithInteractable();
	
private:

	UPROPERTY(EditAnywhere, Category = "Weapon Handler")
	class APW_Character* _ownerCharacter;

	class IPW_InteractableInterface* _lastInteractable;
};
