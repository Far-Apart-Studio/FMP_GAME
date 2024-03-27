// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageIndicatorWidget.generated.h"

USTRUCT(BlueprintType)
struct FIndicateHandle
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) AActor* ownerActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) AActor* damageCauser;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float damageAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float duration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float renderThreshold;
};

UCLASS()
class PROJECT_WEST_API UDamageIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY() FIndicateHandle _indicateHandle;

public:
	UFUNCTION(BlueprintCallable) void SetHitDetails(FIndicateHandle hitDetails) { _indicateHandle = hitDetails; }
	UFUNCTION(BlueprintCallable) void SelfDestruct();
	UFUNCTION(BlueprintCallable) float GetClampedAngle();
	UFUNCTION(BlueprintCallable) float GetDotProductBetweenEnemy();
	UFUNCTION(BlueprintCallable) float GetCrossProductBetweenEnemy();
	UFUNCTION(BlueprintCallable) void GetDotAndCrossProductBetweenEnemy(float& dotProduct, float& crossProduct);

	
	UFUNCTION(BlueprintCallable, Category = "Damage Indicator")

	/**
	* @brief
	* Validates the render criteria for the widget.
	* When the dot product difference is greater than the render threshold,
	* The widgets opacity is set to 0.0f, otherwise its set to 1.0f.
	*/
	
	void ValidateRenderStatus();
};
