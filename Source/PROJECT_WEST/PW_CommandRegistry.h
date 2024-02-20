// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW_Character.h"
#include "PW_ConsoleCommand.h"
#include "PW_CommandRegistry.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API UPW_CommandRegistry : public UPW_ConsoleCommand
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "ConsoleCommands")
	void IncreaseHealth();

	APW_Character* GetPlayerCharacter();

	
	
};
