// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PW_SpawnPoint.generated.h"

UCLASS()
class PROJECT_WEST_API APW_SpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	

	APW_SpawnPoint();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

	FORCEINLINE FVector GetPosition() const { return GetActorLocation(); }
};
