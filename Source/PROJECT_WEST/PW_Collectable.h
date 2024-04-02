// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/PW_InteractableInterface.h"
#include "PW_Collectable.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCollected);

UCLASS()
class PROJECT_WEST_API APW_Collectable : public AActor, public IPW_InteractableInterface 
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, Category = "Collectable")
	UStaticMeshComponent* _meshComponent;
	
public:	
	APW_Collectable();

	UPROPERTY(BlueprintAssignable, Category = "Collectable")
	FOnCollected OnCollected;

	virtual void StartFocus_Implementation(AActor* owner) override;
	virtual void EndFocus_Implementation(AActor* owner) override;
	virtual void StartInteract_Implementation(AActor* owner) override;

	UFUNCTION(BlueprintCallable, Category = "Collectable")
	void SetMeshComponent(UStaticMeshComponent* meshComponent) { _meshComponent = meshComponent; }

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
