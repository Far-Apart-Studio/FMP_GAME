// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PROJECT_WEST/Interfaces/PW_InteractableInterface.h"
#include "PW_ExtractionPoint.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWinDelegate, bool , winCondition, TArray<FString>, escapedPlayers);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerTriggerDelegate, class APW_Character*, player);

UCLASS()
class PROJECT_WEST_API APW_ExtractionPoint : public AActor,public IPW_InteractableInterface
{
	GENERATED_BODY()
	
public:
	
	APW_ExtractionPoint();

protected:
	
	virtual void BeginPlay() override;

private:
	
	bool _triggered;

public:
	
	virtual void Tick(float DeltaTime) override;

	virtual void StartFocus_Implementation(AActor* owner) override;
	virtual void EndFocus_Implementation(AActor* owner) override;
	virtual void StartInteract_Implementation(AActor* owner) override;
	virtual bool HasServerInteraction_Implementation() override;
	virtual void ServerStartInteract_Implementation(AActor* owner) override;

	UFUNCTION()
	virtual void OnEnterExtractionBox (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UPROPERTY( BlueprintReadWrite, VisibleAnywhere )
	bool _canInteract;

	UPROPERTY(VisibleAnywhere)
	class USceneComponent* _root;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* _mesh;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* _becon;
	
	UPROPERTY (VisibleAnywhere)
	class UBoxComponent* _extractionBox;
	
	void CheckForWin();

	UPROPERTY(BlueprintAssignable)
	FWinDelegate OnWinConditionMet;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerTriggerDelegate OnPlayerTrigger;
};
