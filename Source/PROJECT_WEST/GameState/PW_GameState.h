// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "PW_GameState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API APW_GameState : public AGameState
{
	GENERATED_BODY()

public:
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "GameMode" , meta = (AllowPrivateAccess = "true") )
	class UPW_GameInstance* _gameInstance = nullptr;
	
	APW_GameState();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable)
	class APW_PlayerController* GetLocalPlayerController();
};
