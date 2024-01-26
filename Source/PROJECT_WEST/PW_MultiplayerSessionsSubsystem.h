// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "PW_MultiplayerSessionsSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerCreateDelegate, bool, success);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FServerJoinDelegate, bool, success);

UCLASS()
class PROJECT_WEST_API UPW_MultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	
    UPW_MultiplayerSessionsSubsystem();
    void Initialize(FSubsystemCollectionBase& Collection) override;
    void Deinitialize() override;
	
	IOnlineSessionPtr sessionInterface;
	
	UFUNCTION(BlueprintCallable)
	void CreateSession(const FString& serverName, int32 numberOfConnection);

	UFUNCTION(BlueprintCallable)
	void FindSession(const FString& serverName);

	void OnCreateSessionComplete(FName sessionName, bool success);
	void OnStartSessionComplete(FName sessionName, bool success);
	void OnDestroySessionComplete(FName sessionName, bool success);
	void OnFindSessionsComplete(bool success);
	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);

	bool _createSessionAfterDestroy;
	FString _sessionToDestroyName;
	FString _serverNameToFind;
	int32 _numberOfConnectionToCreate;

	TSharedPtr<class FOnlineSessionSearch> _sessionSearch;

	UPROPERTY(BlueprintAssignable)
	FServerCreateDelegate _serverCreateDelegate;

	UPROPERTY(BlueprintAssignable)
	FServerJoinDelegate _serverJoinDelegate;
	
	UPROPERTY (BlueprintReadWrite)
	FString _mapPath;
};
