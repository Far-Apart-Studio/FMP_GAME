// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_Utilities.h"

PW_Utilities::PW_Utilities() {}
PW_Utilities::~PW_Utilities() {}

template <typename T>
T PW_Utilities::GetElement(TArray<T>& inArray)
{
	if (inArray.Num() == 0)
		return nullptr;
	
	return inArray[FMath::RandRange(0, inArray.Num() - 1)];
}

template <typename T>
bool PW_Utilities::GetElement(TArray<T>& inArray, T& outElement)
{
	if (inArray.Num() == 0)
		return false;

	outElement = inArray[FMath::RandRange(0, inArray.Num() - 1)];
	return true;
}

template <typename T>
T PW_Utilities::GetAndRemoveElement(TArray<T>& inArray)
{
	if (inArray.Num() == 0)
		return nullptr;

	int32 index = FMath::RandRange(0, inArray.Num() - 1);
	T element = inArray[index];
	inArray.RemoveAt(index);

	return element;
}

template <typename T>
bool PW_Utilities::IsEmpty(const TArray<T>& inArray)
{
	return inArray.Num() == 0;
}

template <typename T>
bool PW_Utilities::Duplicate(const TArray<T>& inArray, TArray<T>& outArray)
{
	if (inArray.Num() == 0)
		return false;

	for (const T& element : inArray)
		outArray.Add(element);

	return true;
}

template <typename T>
TArray<T> PW_Utilities::Duplicate(const TArray<T>& inArray)
{
	TArray<T> outArray;
	if (inArray.Num() == 0)
		return outArray;

	for (const T& element : inArray)
		outArray.Add(element);

	return outArray;
}

inline void PW_Utilities::Log(const FString& inString)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *inString);
}

inline void PW_Utilities::Log(const FString& inString, const FString& inString2)
{
	UE_LOG(LogTemp, Warning, TEXT("%s %s"), *inString, *inString2);
}

inline void PW_Utilities::Log(const FString& inString, const int32 inInt)
{
	UE_LOG(LogTemp, Warning, TEXT("%s %d"), *inString, inInt);
}

inline void PW_Utilities::Log(const FString& inString, const float inFloat)
{
	UE_LOG(LogTemp, Warning, TEXT("%s %f"), *inString, inFloat);
}

inline void PW_Utilities::Log(const FString& inString, const FVector& inVector)
{
	UE_LOG(LogTemp, Warning, TEXT("%s %s"), *inString, *inVector.ToString());
}

inline void PW_Utilities::Log(const FString& inString, const FRotator& inRotator)
{
	UE_LOG(LogTemp, Warning, TEXT("%s %s"), *inString, *inRotator.ToString());
}

inline void PW_Utilities::Log(const FString& inString, const FTransform& inTransform)
{
	UE_LOG(LogTemp, Warning, TEXT("%s %s"), *inString, *inTransform.ToString());
}
