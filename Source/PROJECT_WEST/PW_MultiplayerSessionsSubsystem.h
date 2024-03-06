// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "PW_MultiplayerSessionsSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSessionCreateDelegate, bool, success);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FSessionJoinDelegate, bool, success);

USTRUCT(BlueprintType)
struct FSessionInfo
{
	GENERATED_BODY()
	
	UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = Info, meta = (AllowPrivateAccess = "true"))
	FString m_sessionName;
	
	UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = Stats, meta = (AllowPrivateAccess = "true"))
	int _currentNumberOfConnection;

	UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = Stats, meta = (AllowPrivateAccess = "true"))
	int _maxNumberOfConnection;
	
	FSessionInfo ()
	{
		m_sessionName = "";
		_currentNumberOfConnection = 0;
		_maxNumberOfConnection = 0;
	}

	FSessionInfo (FString sessionName, int currentNumberOfConnection, int maxNumberOfConnection)
	{
		m_sessionName = sessionName;
		_currentNumberOfConnection = currentNumberOfConnection;
		_maxNumberOfConnection = maxNumberOfConnection;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSessionFindDelegate, bool, success, const TArray<FSessionInfo>&, sessionsInfo);

UCLASS()
class PROJECT_WEST_API UPW_MultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	
    UPW_MultiplayerSessionsSubsystem();
    void Initialize(FSubsystemCollectionBase& Collection) override;
    void Deinitialize() override;

	void OnCreateSessionComplete(FName sessionName, bool success);
	void OnStartSessionComplete(FName sessionName, bool success);
	void OnFindSessionsComplete(bool success);
	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);
	void OnDestroySessionComplete(FName sessionName, bool success);
	
	IOnlineSessionPtr sessionInterface;

	TSharedPtr<FOnlineSessionSettings> _lastSessionSettings;
	
	//FOnlineSessionSettings* _lastSessionSettings;
	
	UFUNCTION(BlueprintCallable)
	void CreateSessionTrigger(int32 numberOfConnection, bool isPublic);
	void CreateSessionDone(bool success);

	UFUNCTION(BlueprintCallable)
	void FindSessionTrigger(const FString& serverName);
	void FindSessionDone(bool success);

	void JoinSessionTrigger(FOnlineSessionSearchResult* sessionToJoin);
	void JoinSessionDone(FName sessionName, bool success);
	bool _isSeachingForSingleSession;

	UFUNCTION(BlueprintCallable)
	void DestroySessionTrigger();
	void DestroySessionDone(bool success);
	bool _createSessionAfterDestroy;
	
	UFUNCTION(BlueprintCallable)
	void FindActivePublicSessionTrigger();
	void FindActivePublicSessionDone(bool success);
	bool _isSeachingForActivePublicSession;

	UFUNCTION(BlueprintCallable)
	void ToggleSessionStatus(bool locked);
	
	FString _sessionToDestroyName;
	FString _serverNameToFind;
	int32 _numberOfConnectionToCreate;
	bool _isPublic;
	
	TSharedPtr<class FOnlineSessionSearch> _sessionSearch;

	UPROPERTY(BlueprintAssignable)
	FSessionCreateDelegate _sessionCreateDelegate;

	UPROPERTY(BlueprintAssignable)
	FSessionJoinDelegate _sessionJoinDelegate;

	UPROPERTY(BlueprintAssignable)
	FSessionFindDelegate _sessionFindDelegate;
	
	UPROPERTY (BlueprintReadWrite)
	FString _mapPath;
};
