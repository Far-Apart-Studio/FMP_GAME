// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PW_HUD.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API APW_HUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
};
