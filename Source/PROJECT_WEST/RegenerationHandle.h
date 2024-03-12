// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/*USTRUCT()
struct FRegenerationHandle
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere) bool AllowRegeneration;
	UPROPERTY(EditAnywhere, NotReplicated) float RegenerationMaximum;
	UPROPERTY(EditAnywhere, NotReplicated) float RegenerationMinimum;
	UPROPERTY(EditAnywhere, NotReplicated) float RegenerationRate;
	UPROPERTY(EditAnywhere, NotReplicated) float RegenerationAmount;
	UPROPERTY(VisibleAnywhere, NotReplicated) float RegenerationTimer;

	DECLARE_DELEGATE_OneParam(FRegenerationMethod, float);
	FRegenerationMethod RegenerationMethod;

	DECLARE_DELEGATE_RetVal(bool, FRegenerationCondition);
	FRegenerationCondition RegenerationCondition;

	FRegenerationHandle()
	{
		AllowRegeneration = false;
		RegenerationMinimum = 0.0f;
		RegenerationMaximum = 100.0f;
		RegenerationRate = 1.0f;
		RegenerationAmount = 2.0f;
		RegenerationTimer = 0.0f;
	}

	void Regenerate(float& currentHealth, float deltaTime)
	{
		RegenerationTimer += deltaTime;

		if (!CanRegenerate(currentHealth))
			return;

		if (RegenerationMethod.IsBound())
		{
			const bool isExecuted = RegenerationMethod.ExecuteIfBound(RegenerationAmount);
			if (isExecuted)
				RegenerationTimer = 0.0f;
		}
		else
		{
			currentHealth = FMath::Clamp
			(currentHealth + RegenerationAmount, RegenerationMinimum, RegenerationMaximum);
			RegenerationTimer = 0.0f;
		}
	}

	bool CanRegenerate(float currentValue)
	{
		const bool canRegenerate =
			currentValue < RegenerationMaximum
			&& AllowRegeneration
			&& RegenerationTimer > RegenerationRate
			&& RegenerationAmount > 0.0f;

		if (RegenerationCondition.IsBound())
			return canRegenerate && RegenerationCondition.Execute();

		return canRegenerate;
	}
};*/
