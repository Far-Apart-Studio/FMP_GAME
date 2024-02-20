// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_CommandRegistry.h"
#include "DebugMacros.h"
#include "PW_Character.h"
#include "PW_Utilities.h"
#include "PlayerController/PW_PlayerController.h"

void UPW_CommandRegistry::IncreaseHealth()
{
	const APW_Character* playerCharacter = GetPlayerCharacter();
	
	if (playerCharacter == nullptr)
		{ PW_Utilities::Log("PLAYER CHARACTER IS NULL!"); return; }
	
}

APW_Character* UPW_CommandRegistry::GetPlayerCharacter()
{
	AActor* ownerActor = GetOwner();

	if (ownerActor == nullptr)
		return nullptr;
	
	const APW_PlayerController* playerController = Cast<APW_PlayerController>(ownerActor);

	if (playerController == nullptr)
		return nullptr; 
 
	APawn* playerPawn = playerController->GetPawn();

	if (playerPawn == nullptr)
		return nullptr; 

	APW_Character* playerCharacter = Cast<APW_Character>(playerPawn);

	if (playerCharacter == nullptr)
		return nullptr; 

	return playerCharacter;
}
