// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PW_ExtractionPoint.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FWinDelegate, bool , winCondition );

UCLASS()
class PROJECT_WEST_API APW_ExtractionPoint : public AActor
{
	GENERATED_BODY()
	
public:
	
	APW_ExtractionPoint();

protected:
	
	virtual void BeginPlay() override;

public:
	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY( BlueprintReadWrite, VisibleAnywhere )
	bool _winCondition;

	UPROPERTY(VisibleAnywhere)
	class USceneComponent* _root;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* _mesh;
	
	UPROPERTY (VisibleAnywhere)
	class UBoxComponent* _extractionBox;

	void CheckForWin();

	UFUNCTION(NetMulticast,Reliable)
	void MulticastRPCWin();

	UPROPERTY(BlueprintAssignable)
	FWinDelegate OnWinConditionMet;

};
