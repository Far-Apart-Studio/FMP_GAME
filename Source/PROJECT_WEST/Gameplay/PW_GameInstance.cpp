// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_GameInstance.h"
#include "PROJECT_WEST/DebugMacros.h"

UPW_GameInstance::UPW_GameInstance()
{
	
}

void UPW_GameInstance::Init()
{
	Super::Init();
	DEBUG_STRING("GameInstance Init");
}

void UPW_GameInstance::OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld)
{
	Super::OnWorldChanged(OldWorld, NewWorld);
	FString OldWorldName = OldWorld ? OldWorld->GetName() : "nullptr";
	FString NewWorldName = NewWorld ? NewWorld->GetName() : "nullptr";
	DEBUG_STRING("GameInstance World Changed from " + OldWorldName + " to " + NewWorldName);
}

void UPW_GameInstance::Shutdown()
{
	DEBUG_STRING("GameInstance Shutdown");
	Super::Shutdown();
}
