// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PROJECT_WEST/Interfaces//PW_InteractableInterface.h"
#include "PW_InteractableActor.generated.h"

UCLASS()
class PROJECT_WEST_API APW_InteractableActor : public AActor , public IPW_InteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APW_InteractableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
