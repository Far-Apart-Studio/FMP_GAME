// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PW_PlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bPingTooHigh);

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API APW_PlayerController : public APlayerController
{
	GENERATED_BODY()


public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual float GetServerTime(); // Synced with server world clock
	virtual void ReceivedPlayer() override; // Sync with server clock as soon as possible

	FHighPingDelegate HighPingDelegate;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:

void SetHUDHealth(float health, float maxHealth);
void SetHUDScore(float score);
};
