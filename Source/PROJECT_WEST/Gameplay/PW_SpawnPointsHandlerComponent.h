// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PW_SpawnPointsHandlerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_SpawnPointsHandlerComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPW_SpawnPointsHandlerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// make _lanternSpawnPoint moveable in editor with a gizmo
	UPROPERTY(EditAnywhere, Category = "SpawnPoints", Meta = (MakeEditWidget = true))
	FVector _lanternSpawnPoint;

	FVector GetLanternSpawnPoint();

private:

	FVector ConvertToWorldLocation(FVector location);
};
