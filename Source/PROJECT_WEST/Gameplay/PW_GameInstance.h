// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PROJECT_WEST/Bounty System/PW_BountyData.h"
#include "PW_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API UPW_GameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Bounty System" , meta = (AllowPrivateAccess = "true") )
	FBountyDataEntry _bountyDataEntry;

protected:

	UPW_GameInstance();

	void Init() override;
	void OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld) override;
	void Shutdown() override;

};
