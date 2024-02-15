// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_ConsoleCommandManager.h"

#include "PW_ConsoleCommand.h"
#include "PW_Utilities.h"
#include "Net/UnrealNetwork.h"

UPW_ConsoleCommandManager::UPW_ConsoleCommandManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool UPW_ConsoleCommandManager::ProcessConsoleExec(const TCHAR* Command, FOutputDevice& Ar, UObject* Executor)
{
	for (UPW_ConsoleCommand* Object : _consoleCommandsInstances)
	{
		if (Object->ProcessConsoleExec(Command, Ar, Executor))
			return true;
	}

	return Super::ProcessConsoleExec(Command, Ar, Executor);
}

void UPW_ConsoleCommandManager::BeginPlay()
{
	Super::BeginPlay();

	const AActor* ownerActor = GetOwner();
	if (ownerActor == nullptr)
		PW_Utilities::Log("COULD NOT FIND OWNER ACTOR FOR CONSOLE COMMAND MANAGER");

	if (!ownerActor->HasAuthority())
		return;

	for (const TSubclassOf<UPW_ConsoleCommand>& classType : _consoleCommandsClasses)
	{
		if (classType != nullptr)
			_consoleCommandsInstances.Add(NewObject<UPW_ConsoleCommand>
				(GetOwner(), classType));
	}
}

void UPW_ConsoleCommandManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPW_ConsoleCommandManager, _consoleCommandsInstances);
}

void UPW_ConsoleCommandManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

