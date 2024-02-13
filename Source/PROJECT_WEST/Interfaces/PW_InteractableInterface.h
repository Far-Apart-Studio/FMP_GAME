// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PW_InteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPW_InteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECT_WEST_API IPW_InteractableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void StartFocus();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void EndFocus();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void StartInteract(AActor* owner);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void EndInteract();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	bool IsInteracting();
};
