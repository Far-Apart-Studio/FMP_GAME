// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PW_GameMode.h"
#include "PW_LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API APW_LobbyGameMode : public APW_GameMode
{
	GENERATED_BODY()

protected:
	
	APW_LobbyGameMode();
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Lobby" )
	FString _mapPath;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Lobby" )
	class UBountySystemComponent* _bountySystemComponent;

	class APW_BountyBoard* _bountyBoard;

public:
	void BeginPlay() override;
	FORCEINLINE UBountySystemComponent* GetBountySystemComponent() const { return _bountySystemComponent; }
};
