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

class PROJECT_WEST_API IPW_InteractableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	bool CanInteract();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void StartFocus(AActor* owner);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void EndFocus(AActor* owner);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void StartInteract(AActor* owner);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void EndInteract();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	bool IsInteracting();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	bool HasServerInteraction();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void ServerStartInteract(AActor* owner);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void ServerStopInteract();
};
