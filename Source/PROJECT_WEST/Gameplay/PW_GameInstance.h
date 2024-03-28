// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PROJECT_WEST/Gameplay/GameSessionData.h"
#include "PW_GameInstance.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FOnLoadingEvent);


UCLASS()
class PROJECT_WEST_API UPW_GameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Bounty System" , meta = (AllowPrivateAccess = "true") )
	FGameSessionData _gameSessionData;

protected:

	UPW_GameInstance();

	void Init() override;

	UFUNCTION() virtual void BeginLoadingScreen(const FString& MapName);
	UFUNCTION() virtual void EndLoadingScreen(UWorld* InLoadedWorld);
	
	void OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld) override;

	void Shutdown() override;

public:

	UFUNCTION(BlueprintCallable)
	void ResetGameData() { _gameSessionData = FGameSessionData(); }
	FORCENOINLINE FGameSessionData& GetGameSessionData() { return _gameSessionData; }
	
	UPROPERTY (BlueprintAssignable)
	FOnLoadingEvent _OnLoadingStart;

	UPROPERTY (BlueprintAssignable)
	FOnLoadingEvent _OnLoadingEnd;
};
