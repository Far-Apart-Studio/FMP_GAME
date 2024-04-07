// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "DebugMacros.h"

UPW_MultiplayerSessionsSubsystem::UPW_MultiplayerSessionsSubsystem(): _isSeachingForSingleSession(false),
                                                                      _createSessionAfterDestroy(false),
                                                                      _isSeachingForActivePublicSession(false),
                                                                      _numberOfConnectionToCreate(0), _isPublic(false)
{
}

void UPW_MultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (const IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get())
	{
		const FString subsystemName = onlineSubsystem->GetSubsystemName().ToString();
		DEBUG_STRING(subsystemName);

		sessionInterface = onlineSubsystem->GetSessionInterface();
		if (sessionInterface.IsValid())
		{
			sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPW_MultiplayerSessionsSubsystem::OnCreateSessionComplete);
			sessionInterface->OnStartSessionCompleteDelegates.AddUObject(this, &UPW_MultiplayerSessionsSubsystem::OnStartSessionComplete);
			sessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPW_MultiplayerSessionsSubsystem::OnDestroySessionComplete);
			sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPW_MultiplayerSessionsSubsystem::OnFindSessionsComplete);
			sessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPW_MultiplayerSessionsSubsystem::OnJoinSessionComplete);
		}
	}
}

void UPW_MultiplayerSessionsSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UPW_MultiplayerSessionsSubsystem::OnCreateSessionComplete(FName sessionName, bool success)
{
	CreateSessionDone(success);
}

void UPW_MultiplayerSessionsSubsystem::OnStartSessionComplete(FName sessionName, bool success)
{
	
}

void UPW_MultiplayerSessionsSubsystem::OnFindSessionsComplete(bool success)
{
	//PrintString ("OnFindSessionsComplete " + FString::FromInt(success) + " " + FString::FromInt(_isSeachingForSingleSession) + " " + FString::FromInt(_isSeachingForActivePublicSession));
	
	if(_isSeachingForSingleSession)
	{
		_isSeachingForSingleSession = false;
		FindSessionDone(success);
	}
	
	if (_isSeachingForActivePublicSession)
	{
		_isSeachingForActivePublicSession = false;
		FindActivePublicSessionDone(success);
	}
}

void UPW_MultiplayerSessionsSubsystem::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result) const
{
	JoinSessionDone(sessionName, result == EOnJoinSessionCompleteResult::Success);
}

void UPW_MultiplayerSessionsSubsystem::OnDestroySessionComplete(FName sessionName, bool success)
{
	DestroySessionDone(success);
}

void UPW_MultiplayerSessionsSubsystem::CreateSessionTrigger(int32 numberOfConnection, bool isPublic)
{
	_isPublic = isPublic;

	const FString& sessionType = isPublic ? "Public" : "Private";
	
	if (numberOfConnection == 0)
	{
		DEBUG_STRING("numberOfConnection is 0");
		_sessionCreateDelegate.Broadcast(false);
		return;
	}
	
	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	const FString& serverName = localPlayer->GetNickname() +  FString::FromInt(FMath::RandRange(0, 99));
	const FString& sessionStatus = "Open";
	
	DEBUG_STRING(serverName);
	
	_numberOfConnectionToCreate = numberOfConnection;

	if (FNamedOnlineSession *ExistingSession =  sessionInterface->GetNamedSession(NAME_GameSession))
	{
		DEBUG_STRING( "Session Already Exists" );
		_createSessionAfterDestroy = true;
		_sessionToDestroyName = serverName;
		DestroySessionTrigger();
		return;	
	}

	//_lastSessionSettings = new FOnlineSessionSettings();
	_lastSessionSettings = MakeShareable(new FOnlineSessionSettings());

	_lastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	
	_lastSessionSettings->NumPublicConnections = _numberOfConnectionToCreate;
	
	_lastSessionSettings->bAllowJoinInProgress = true;
	_lastSessionSettings->bAllowJoinViaPresence = true;
	_lastSessionSettings->bIsDedicated = false;
	_lastSessionSettings->bShouldAdvertise = true;
	_lastSessionSettings->bUseLobbiesIfAvailable = true;
	_lastSessionSettings->bUsesPresence = true;
	_lastSessionSettings->BuildUniqueId = 1;

	_lastSessionSettings->Set(FName("Session_Type"), sessionType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	_lastSessionSettings->Set(FName("Session_Name"), serverName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	_lastSessionSettings->Set(FName("Session_Status"), sessionStatus, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	sessionInterface->CreateSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *_lastSessionSettings);

	_processingDelegate.Broadcast(true);
}

void UPW_MultiplayerSessionsSubsystem::CreateSessionDone(bool success)
{
	_sessionCreateDelegate.Broadcast(success);
	_processingDelegate.Broadcast(false);
	
	if (success)
	{
		DEBUG_STRING ("Session Created");

		_OnMapTransition.Broadcast();
		
		FString path = "/Game/ThirdPerson/Maps/ThirdPersonMap";
		if (_mapPath.IsEmpty())
		{
			_mapPath = path;
		}
		GetWorld()->ServerTravel( _mapPath + "?listen");
	}
}

void UPW_MultiplayerSessionsSubsystem::JoinSessionTrigger(const FOnlineSessionSearchResult* sessionToJoin) const
{
	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	sessionInterface->JoinSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *sessionToJoin);
}

void UPW_MultiplayerSessionsSubsystem::JoinSessionDone(FName sessionName, bool success) const
{
	_processingDelegate.Broadcast(false);
	
	if(!success)
	{
		_sessionJoinDelegate.Broadcast(success);
		DEBUG_STRING("Join Session Failed: " + sessionName.ToString());
		return;
	}
	
	FString address = "";
	success = sessionInterface->GetResolvedConnectString(NAME_GameSession, address);
	if (success)
	{
		if (APlayerController* playerController = GetGameInstance()->GetFirstLocalPlayerController())
		{
			_OnMapTransition.Broadcast();
			playerController->ClientTravel(address, ETravelType::TRAVEL_Absolute);
		}
	}
	else
	{
		_sessionJoinDelegate.Broadcast(false);
		DEBUG_STRING("Could Not Get Connect String");
	}
}

void UPW_MultiplayerSessionsSubsystem::FindSessionTrigger(const FString& serverName)
{
	DEBUG_STRING ("Find Server Called : " + serverName);

	if (serverName.IsEmpty())
	{
		DEBUG_STRING("Server Name is Empty");
		_sessionJoinDelegate.Broadcast(false);
		return;
	}

	_sessionSearch = MakeShareable(new FOnlineSessionSearch());
	_sessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	_sessionSearch->MaxSearchResults = 9999;
	_sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	_serverNameToFind = serverName;
	_isSeachingForSingleSession = true;
	
	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	sessionInterface->FindSessions(*localPlayer->GetPreferredUniqueNetId(), _sessionSearch.ToSharedRef());

	_processingDelegate.Broadcast(true);
}

void UPW_MultiplayerSessionsSubsystem::FindSessionDone(bool success) const
{
	if (!success || _serverNameToFind.IsEmpty())
	{
		DEBUG_STRING("Find Session Failed - FindSessionDone");
		_sessionJoinDelegate.Broadcast(false);
		return;
	}

	TArray<FOnlineSessionSearchResult> searchResults = _sessionSearch->SearchResults;

	if (searchResults.Num() > 0)
	{
		const FOnlineSessionSearchResult* correctResult = nullptr;
		for (FOnlineSessionSearchResult result : searchResults)
		{
			if (result.IsValid())
			{
				FString serverName = "No Name";
				FString sessionStatus = "";
				result.Session.SessionSettings.Get(FName("Session_Name"), serverName);
				result.Session.SessionSettings.Get(FName("Session_Status"), sessionStatus);

				if (serverName.Equals(_serverNameToFind) && sessionStatus.Equals("Open"))
				{
					correctResult = &result;
					break;
				}
			}
		}

		if (correctResult)
		{
			const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			sessionInterface->JoinSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *correctResult);
		}
		else
		{
			DEBUG_STRING("Correct Session Not Found");
			_sessionJoinDelegate.Broadcast(false);
		}
	}
	else
	{
		DEBUG_STRING("No Sessions Found");
		_sessionJoinDelegate.Broadcast(false);
	}
	_sessionSearch->SearchResults.Empty();
}

void UPW_MultiplayerSessionsSubsystem::DestroySessionTrigger()
{
	sessionInterface->DestroySession(NAME_GameSession);
}

void UPW_MultiplayerSessionsSubsystem::DestroySessionDone(bool success)
{
	DEBUG_STRING ("Session Destroyed");
	if (success && _createSessionAfterDestroy)
	{
		_createSessionAfterDestroy = false;
		CreateSessionTrigger( _numberOfConnectionToCreate , _isPublic);
	}
}

void UPW_MultiplayerSessionsSubsystem::FindActivePublicSessionTrigger()
{
	DEBUG_STRING( "Find Active Public Session Called" );
	
	//_sessionFindDelegate.Broadcast(false, TArray<FSessionInfo>());
	
	_sessionSearch = MakeShareable(new FOnlineSessionSearch());
	_sessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	_sessionSearch->MaxSearchResults = 9999;
	_sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	_isSeachingForActivePublicSession = true;
	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	sessionInterface->FindSessions(*localPlayer->GetPreferredUniqueNetId(), _sessionSearch.ToSharedRef());

	_processingDelegate.Broadcast(true);
}

void UPW_MultiplayerSessionsSubsystem::FindActivePublicSessionDone(bool success) const
{
	_processingDelegate.Broadcast(false);
	
	if (!success)
	{
		DEBUG_STRING("Find Session Failed - FindActivePublicSessiontDone");
		_sessionFindDelegate.Broadcast(false, TArray<FSessionInfo>());
		return;
	}

	TArray<FOnlineSessionSearchResult> searchResults = _sessionSearch->SearchResults;
	TArray<FSessionInfo> sessionInfos = TArray<FSessionInfo>();
	
	if (searchResults.Num() > 0)
	{
		for (FOnlineSessionSearchResult result : searchResults)
		{
			if (result.IsValid())
			{
				FString serverName = "No Name";
				FString sessionType = "No Type";
				FString sessionStatus = "Open";
				result.Session.SessionSettings.Get(FName("Session_Name"), serverName);
				result.Session.SessionSettings.Get(FName("Session_Type"), sessionType);
				result.Session.SessionSettings.Get(FName("Session_Status"), sessionStatus);

				if (serverName.Equals( "No Name") || sessionType.Equals("Private") || sessionStatus.Equals("Closed"))
				{
					continue;
				}

				sessionInfos.Add(FSessionInfo(serverName, result.Session.NumOpenPublicConnections, result.Session.SessionSettings.NumPublicConnections));
			}
		}

		_sessionFindDelegate.Broadcast(true,sessionInfos);
	}
	else
	{
		_sessionFindDelegate.Broadcast(false,sessionInfos);
		DEBUG_STRING("No Sessions Found : FindActivePublicSessionDone");
	}
	_sessionSearch->SearchResults.Empty();
}

void UPW_MultiplayerSessionsSubsystem::ToggleSessionStatus(bool locked)
{
	 const FString& sessionStatus = locked ? "Closed" : "Open";
	 if (sessionInterface && _lastSessionSettings)
	 {
	 	DEBUG_STRING("Session Status Changed");
	 	//_lastSessionSettings->Set(FName("Session_Status"), sessionStatus, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	 	//sessionInterface->UpdateSession(NAME_GameSession, *_lastSessionSettings);
	 }
}



