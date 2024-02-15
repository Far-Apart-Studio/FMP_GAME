// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_ConsoleCommand.h"

UPW_ConsoleCommand::UPW_ConsoleCommand()
{
	SetIsReplicatedByDefault(true);
	for (TFieldIterator<UFunction> It(GetClass()); It; ++It)
	{
		if (GetClass()->IsChildOf(Cast<UClass>(It.GetStruct())))
			It->FunctionFlags |= FUNC_Exec;
	}
}

void UPW_ConsoleCommand::BeginPlay()
{
	Super::BeginPlay();
}

void UPW_ConsoleCommand::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

