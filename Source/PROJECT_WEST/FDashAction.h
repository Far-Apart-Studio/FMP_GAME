// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LatentActions.h"
#include "PWMath.h"
#include "PW_Character.h"
#include "GameFramework/CharacterMovementComponent.h"

class FDashAction : public FPendingLatentAction
{
public:
	float _timeRemaining;
	float _dashDuration;
	float _dashSpeed;
	FVector _dashDirection;
	UCurveFloat* _dashCurve;
	APW_Character* characterObject;
	FName _executionFunction;
	FWeakObjectPtr _callbackTarget;
	
	FDashAction(float duration, float dashSpeed, const FVector& dashDirection, UCurveFloat* curveFloat ,APW_Character* characterObject, const FLatentActionInfo& latentInfo)
		: _timeRemaining(duration)
		, _dashDuration(duration)
		, _dashSpeed(dashSpeed)
		, _dashDirection(dashDirection)
		, _dashCurve(curveFloat)
		, characterObject(characterObject)
		, _executionFunction(latentInfo.ExecutionFunction)
		, _callbackTarget(latentInfo.CallbackTarget)
	{
	}

	virtual void UpdateOperation(FLatentResponse& response) override
	{
		_timeRemaining -= response.ElapsedTime();

		const float normalisedTime = PWMath::Clamp01(1.0f - (_timeRemaining / _dashDuration));
		float dashCurveValue = 1.0f;
		
		if (_dashCurve != nullptr)
			dashCurveValue = _dashCurve->GetFloatValue(normalisedTime);
		
		const float actualDashSpeed = _dashSpeed * dashCurveValue;
		const FVector dashThisFrame = _dashDirection * actualDashSpeed;
		
		characterObject->GetCharacterMovement()->Velocity = dashThisFrame;
		response.FinishAndTriggerIf(_timeRemaining <= 0.0f, _executionFunction, 0 , _callbackTarget);
	}
};
