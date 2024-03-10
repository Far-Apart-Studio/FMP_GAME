// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PROJECT_WEST/Interfaces/PW_InteractableInterface.h"
#include "PW_ExtractionPoint.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FWinDelegate, bool , winCondition, TArray<FString>, escapedPlayers);

UCLASS()
class PROJECT_WEST_API APW_ExtractionPoint : public AActor,public IPW_InteractableInterface
{
	GENERATED_BODY()
	
public:
	
	APW_ExtractionPoint();

protected:
	
	virtual void BeginPlay() override;

public:
	
	virtual void Tick(float DeltaTime) override;

	virtual void StartFocus_Implementation() override;
	virtual void EndFocus_Implementation() override;
	virtual void StartInteract_Implementation(AActor* owner) override;
	virtual bool HasServerInteraction_Implementation() override;
	virtual void ServerStartInteract_Implementation(AActor* owner) override;

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

};
