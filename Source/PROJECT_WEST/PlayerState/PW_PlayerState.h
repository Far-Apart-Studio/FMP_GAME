// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PW_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API APW_PlayerState : public APlayerState
{
	GENERATED_BODY()
public:

	APW_PlayerState();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Info" )
	int32 _colorIndex;

private:
	class APW_Character* _character;
	class APW_PlayerController* _playerController;

	UFUNCTION( Client, Reliable ) void ClientSetColorIndex(int index);
};
