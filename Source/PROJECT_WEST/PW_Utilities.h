// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class PROJECT_WEST_API PW_Utilities
{
public:
	PW_Utilities();
	~PW_Utilities();

	template<typename T>
	static T GetElement(TArray<T> &inArray);
	
	template<typename T>
	static bool GetElement(TArray<T>& inArray, T& outElement);
	
	template<typename T>
	static T GetAndRemoveElement(TArray<T> &inArray);

	template<typename T>
	static bool IsEmpty(const TArray<T>& inArray);

	template <typename T>
	static bool Duplicate(const TArray<T>& inArray, TArray<T>& outArray);

	template <typename T>
	static TArray<T> Duplicate(const TArray<T>& inArray);
	
	static void Log(const FString& inString);
	static void Log(const FString& inString, const FString& inString2);
	static void Log(const FString& inString, int32 inInt);
	static void Log(const FString& inString, float inFloat);
	static void Log(const FString& inString, const FVector& inVector);
	static void Log(const FString& inString, const FRotator& inRotator);
	static void Log(const FString& inString, const FTransform& inTransform);
};
