// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class PROJECT_WEST_API PWMath
{
public:
	PWMath();
	~PWMath();

	FORCEINLINE static float Clamp01(float value) { return FMath::Clamp(value, 0.0f, 1.0f); }
};
