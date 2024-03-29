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

	UFUNCTION()
	void PawnSet(APlayerState* Player, APawn* NewPawn, APawn* OldPawn);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void SetColorIndex(int32 index);

private:
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Info" ,meta = (AllowPrivateAccess = "true") )
	int32 _colorIndex;

	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "Info" ,meta = (AllowPrivateAccess = "true") )
	class APW_Character* _character;

	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, Category = "Info" ,meta = (AllowPrivateAccess = "true") )
	class APW_PlayerController* _playerController;

};
