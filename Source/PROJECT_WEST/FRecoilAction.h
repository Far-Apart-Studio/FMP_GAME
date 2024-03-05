// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LatentActions.h"
#include "PW_Character.h"
#include "Kismet/GameplayStatics.h"

/**
 * 
 */
class FRecoilAction : public FPendingLatentAction
{
public:
	float timeRemaining;
	FVector recoilAmount;
	APW_Character* characterObject;
	FName ExecutionFunction;
	FWeakObjectPtr CallbackTarget;

	FRecoilAction(float duration, const FVector& recoilAmount, APW_Character* characterObject, const FLatentActionInfo& latentInfo)
		: timeRemaining(duration)
		, recoilAmount(recoilAmount)
		, characterObject(characterObject)
		, ExecutionFunction(latentInfo.ExecutionFunction)
		, CallbackTarget(latentInfo.CallbackTarget)
	{ }

	virtual void UpdateOperation(FLatentResponse& Response) override
	{
		timeRemaining -= Response.ElapsedTime();
		
		const float verticalRecoilThisFrame = recoilAmount.Y * Response.ElapsedTime();
		const float horizontalRecoilThisFrame = recoilAmount.X * Response.ElapsedTime();
		
		characterObject->AddControllerYawInput(horizontalRecoilThisFrame);
		characterObject->AddControllerPitchInput(verticalRecoilThisFrame);
		
		Response.DoneIf(timeRemaining <= 0.0f);
	}
};
