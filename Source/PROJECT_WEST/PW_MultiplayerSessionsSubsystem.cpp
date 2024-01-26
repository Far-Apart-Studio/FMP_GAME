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

void UPW_MultiplayerSessionsSubsystem::CreateSession(const FString& serverName, int32 numberOfConnection)
{
	_numberOfConnectionToCreate = numberOfConnection;

	if (serverName.IsEmpty())
	{
		PrintString("Server Name is Empty");
		_serverCreateDelegate.Broadcast(false);
		return;
	}

	FNamedOnlineSession *ExistingSession =  sessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession)
	{
		PrintString( "Session Already Exists" );
		_createSessionAfterDestroy = true;
		_sessionToDestroyName = serverName;
		sessionInterface->DestroySession(NAME_GameSession);
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

	//sessionSettings.Set(FName("Session_Type"), "Bounty", EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	sessionSettings.Set(FName("SERVER_NAME"), serverName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	sessionInterface->CreateSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, sessionSettings);
}

void UPW_MultiplayerSessionsSubsystem::FindSession(const FString& serverName)
{
	PrintString ("Find Server Called : " + serverName);

	if (serverName.IsEmpty())
	{
		PrintString("Server Name is Empty");
		_serverJoinDelegate.Broadcast(false);
		return;
	}
	
	_sessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	_sessionSearch->MaxSearchResults = 9999;
	_sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	_serverNameToFind = serverName;
	
	sessionInterface->FindSessions(0, _sessionSearch.ToSharedRef());
}

void UPW_MultiplayerSessionsSubsystem::OnCreateSessionComplete(FName sessionName, bool success)
{
	_serverCreateDelegate.Broadcast(success);
	
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

void UPW_MultiplayerSessionsSubsystem::OnStartSessionComplete(FName sessionName, bool success)
{
}

void UPW_MultiplayerSessionsSubsystem::OnDestroySessionComplete(FName sessionName, bool success)
{
	PrintString ("Session Destroyed");
	if (success && _createSessionAfterDestroy)
	{
		_createSessionAfterDestroy = false;
		CreateSession(_sessionToDestroyName, _numberOfConnectionToCreate);
	}
}

void UPW_MultiplayerSessionsSubsystem::OnFindSessionsComplete(bool success)
{
	if (!success || _serverNameToFind.IsEmpty())
	{
		PrintString("Find Session Failed");
		_serverJoinDelegate.Broadcast(false);
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
				result.Session.SessionSettings.Get(FName("SERVER_NAME"), serverName);

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
			_serverJoinDelegate.Broadcast(false);
		}
	}
	else
	{
		PrintString("No Sessions Found");
		_serverJoinDelegate.Broadcast(false);
	}
	_sessionSearch->SearchResults.Empty();
}

void UPW_MultiplayerSessionsSubsystem::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	_serverJoinDelegate.Broadcast(result == EOnJoinSessionCompleteResult::Type::Success);
	
	if (result == EOnJoinSessionCompleteResult::Type::Success)
    {
		FString address = "";
		bool success = sessionInterface->GetResolvedConnectString(NAME_GameSession, address);
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
	else
	{
		PrintString( "Join Session Failed: " + sessionName.ToString());
    }
}
