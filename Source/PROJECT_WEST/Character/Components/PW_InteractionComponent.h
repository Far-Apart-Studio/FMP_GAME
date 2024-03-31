// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PW_InteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractDelegate, AActor*, interactedActor);

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

	UFUNCTION(BlueprintCallable)
	void TryStartInteractWithInteractable();
	
	UFUNCTION(BlueprintCallable)
	void TryEndInteractWithInteractable();

	void StartServerInteractWithInteractable(AActor* _interactedActor,AActor* _interactableOwner);

	UFUNCTION(Server, Reliable)
	void ServerStartInteractWithInteractable(AActor* _interactedActor,AActor* _interactableOwner);
	
	void LocalStartInteractWithInteractable(AActor* _interactedActor,AActor* _interactableOwner);
	

	void EndServerInteractWithInteractable(AActor* _interactedActor);

	UFUNCTION(Server, Reliable)
	void ServerEndInteractWithInteractable(AActor* _interactedActor);
	
	void LocalEndInteractWithInteractable(AActor* _interactedActor);

	UFUNCTION()
	void TryToggleInteract();

	UPROPERTY(BlueprintAssignable)
	FInteractDelegate _OnTraceInteractable;
	
private:

	UPROPERTY(EditAnywhere)
	class APW_Character* _ownerCharacter;

	UPROPERTY(VisibleAnywhere)
	AActor* _lastIntractableActor;

	UPROPERTY(EditAnywhere, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	float _traceDistance;
};
