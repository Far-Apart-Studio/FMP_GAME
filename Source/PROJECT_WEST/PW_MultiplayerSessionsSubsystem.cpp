// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "DebugMacros.h"

UPW_MultiplayerSessionsSubsystem::UPW_MultiplayerSessionsSubsystem()
{
	_createSessionAfterDestroy = false;
	_sessionToDestroyName = "";
	_serverNameToFind = "";
	_mapPath = "";
}

void UPW_MultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get();
	if (onlineSubsystem)
	{
		FString subsystemName = onlineSubsystem->GetSubsystemName().ToString();
		PrintString(subsystemName);

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

void UPW_MultiplayerSessionsSubsystem::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
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
		PrintString("numberOfConnection is 0");
		_sessionCreateDelegate.Broadcast(false);
		return;
	}
	
	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	const FString& serverName = localPlayer->GetNickname() +  FString::FromInt(FMath::RandRange(0, 99));

	PrintString(serverName);
	
	_numberOfConnectionToCreate = numberOfConnection;
	
	FNamedOnlineSession *ExistingSession =  sessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession)
	{
		PrintString( "Session Already Exists" );
		_createSessionAfterDestroy = true;
		_sessionToDestroyName = serverName;
		DestroySessionTrigger();
		return;	
	}
	
	FOnlineSessionSettings sessionSettings;

	sessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	
	sessionSettings.NumPublicConnections = _numberOfConnectionToCreate;
	
	sessionSettings.bAllowJoinInProgress = true;
	sessionSettings.bAllowJoinViaPresence = true;
	sessionSettings.bIsDedicated = false;
	sessionSettings.bShouldAdvertise = true;
	sessionSettings.bUseLobbiesIfAvailable = true;
	sessionSettings.bUsesPresence = true;
	sessionSettings.BuildUniqueId = 1;

	sessionSettings.Set(FName("Session_Type"), sessionType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	sessionSettings.Set(FName("Session_Name"), serverName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	sessionInterface->CreateSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, sessionSettings);
}

void UPW_MultiplayerSessionsSubsystem::CreateSessionDone(bool success)
{
	_sessionCreateDelegate.Broadcast(success);
	
	if (success)
	{
		PrintString ("Session Created");
		FString path = "/Game/ThirdPerson/Maps/ThirdPersonMap";
		if (_mapPath.IsEmpty())
		{
			_mapPath = path;
		}
		GetWorld()->ServerTravel( _mapPath + "?listen");
	}
}

void UPW_MultiplayerSessionsSubsystem::JoinSessionTrigger(FOnlineSessionSearchResult* sessionToJoin)
{
	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	sessionInterface->JoinSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *sessionToJoin);
}

void UPW_MultiplayerSessionsSubsystem::JoinSessionDone(FName sessionName, bool success)
{
	_sessionJoinDelegate.Broadcast(success);

	if(!success)
	{
		PrintString("Join Session Failed: " + sessionName.ToString());
		return;
	}
	
	FString address = "";
	success = sessionInterface->GetResolvedConnectString(NAME_GameSession, address);
	if (success)
	{
		APlayerController* playerController = GetGameInstance()->GetFirstLocalPlayerController();
		if (playerController)
		{
			playerController->ClientTravel(address, ETravelType::TRAVEL_Absolute);
		}
	}
	else
	{
		PrintString("Could Not Get Connect String");
	}
}

void UPW_MultiplayerSessionsSubsystem::FindSessionTrigger(const FString& serverName)
{
	PrintString ("Find Server Called : " + serverName);

	if (serverName.IsEmpty())
	{
		PrintString("Server Name is Empty");
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
}

void UPW_MultiplayerSessionsSubsystem::FindSessionDone(bool success)
{
	if (!success || _serverNameToFind.IsEmpty())
	{
		PrintString("Find Session Failed - FindSessionDone");
		_sessionJoinDelegate.Broadcast(false);
		return;
	}

	TArray<FOnlineSessionSearchResult> searchResults = _sessionSearch->SearchResults;
	FOnlineSessionSearchResult* correctResult = nullptr;
	
	if (searchResults.Num() > 0)
	{
		for (FOnlineSessionSearchResult result : searchResults)
		{
			if (result.IsValid())
			{
				FString serverName = "No Name";
				result.Session.SessionSettings.Get(FName("Session_Name"), serverName);

				if (serverName.Equals(_serverNameToFind))
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
			PrintString("Correct Session Not Found");
			_sessionJoinDelegate.Broadcast(false);
		}
	}
	else
	{
		PrintString("No Sessions Found");
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
	PrintString ("Session Destroyed");
	if (success && _createSessionAfterDestroy)
	{
		_createSessionAfterDestroy = false;
		CreateSessionTrigger( _numberOfConnectionToCreate , _isPublic);
	}
}

void UPW_MultiplayerSessionsSubsystem::FindActivePublicSessionTrigger()
{
	PrintString( "Find Active Public Session Called" );
	
	_sessionFindDelegate.Broadcast(false, TArray<FSessionInfo>());
	
	_sessionSearch = MakeShareable(new FOnlineSessionSearch());
	_sessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	_sessionSearch->MaxSearchResults = 9999;
	_sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	_isSeachingForActivePublicSession = true;
	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	sessionInterface->FindSessions(*localPlayer->GetPreferredUniqueNetId(), _sessionSearch.ToSharedRef());
}

void UPW_MultiplayerSessionsSubsystem::FindActivePublicSessionDone(bool success)
{
	if (!success)
	{
		PrintString("Find Session Failed - FindActivePublicSessiontDone");
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
				result.Session.SessionSettings.Get(FName("Session_Name"), serverName);
				result.Session.SessionSettings.Get(FName("Session_Type"), sessionType);

				if (sessionType.Equals("Private"))
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
		PrintString("No Sessions Found : FindActivePublicSessionDone");
	}
	_sessionSearch->SearchResults.Empty();
}

void UPW_MultiplayerSessionsSubsystem::UpdateSessionInfo()
{
	FOnlineSessionSettings sessionSettings = FOnlineSessionSettings();
	sessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	sessionSettings.NumPublicConnections = 0;
	sessionSettings.NumPrivateConnections = 0;
	sessionInterface->UpdateSession(NAME_GameSession, sessionSettings);
}


