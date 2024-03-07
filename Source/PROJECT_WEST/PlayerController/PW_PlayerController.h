// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PROJECT_WEST/Gameplay/GameSessionData.h"
#include "PW_PlayerController.generated.h"

class UPW_ConsoleCommandManager;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bPingTooHigh);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FVoteChangedDelegate, bool, bsuccess, int32, bountyIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FOnNameChangedDelegate , FString , newName);

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API APW_PlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	
	UPROPERTY(EditAnywhere, Category = "Console Command Manager")
	UPW_ConsoleCommandManager* _consoleCommandManager;

	UPROPERTY( EditAnywhere, Category = "Widgets", meta = (AllowPrivateAccess = "true") )
	TSubclassOf<class UUserWidget> _characterOverlayWidgetClass;

	UPROPERTY( VisibleAnywhere, Category = "Widgets", meta = (AllowPrivate= "true") )
	class UPW_CharacterOverlayWidget* _characterOverlayWidget;

	UPROPERTY( EditAnywhere, Category = "Widgets", meta = (AllowPrivate = "true") )
	TSubclassOf<class UUserWidget> _announcementWidgetClass;

	UPROPERTY( VisibleAnywhere, Category = "Widgets", meta = (AllowPrivate= "true") )
	class UPW_AnnouncementWidget*  _announcementWidget;
	
	class APW_HUD* _hud;

	FTimerHandle _announcementTimer;
	
	float _highPingRunningTime;
	float _pingAnimationRunningTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ping", meta = (AllowPrivateAccess = "true"))
	float _highPingDuration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ping", meta = (AllowPrivateAccess = "true"))
	float _checkPingFrequency;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ping", meta = (AllowPrivateAccess = "true"))
	float _highPingThreshold;

	float _levelStartTime;
	
	float _matchTime;
	
	float _endMatchCountdown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess = "true"))
	float _money;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess = "true"))
	float _dayIndex;
	
	uint32 _countDownInt;

	bool _hasVoted;
	int32 _votedBountyIndex;
	
	bool _voteSeverDelay;
	
	class APW_BountyGameMode* _bountyGameMode;
	class APW_LobbyGameMode* _lobbyGameMode;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName _matchState;

	FString _matchTimeString;

	UPROPERTY(ReplicatedUsing = OnRep_PlayerName,VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess = "true"))
	FString _playerName;

	float _clientServerDelta; // Difference between client and server time

	UPROPERTY(EditAnywhere, Category = "Time", meta = (AllowPrivateAccess = "true"))
	float _timeSyncFrequency; // How often to sync client and server time

	UPROPERTY(VisibleAnywhere, Category = "Time", meta = (AllowPrivateAccess = "true"))
	float _timeSyncRuningTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess = "true"))
	FString _timeText;

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category = "Info", meta = (AllowPrivateAccess = "true"))
	FOnNameChangedDelegate _onNameChanged;

protected:
	
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UFUNCTION( Client, Reliable )
	void ClientAddCharacterOverlayWidget();

	UFUNCTION( Client, Reliable )
	void ClientOnLevelChanged();
	
	void StartHighPingWarning();
	void StopHighPingWarning();
	void HandleCheckPing(float DeltaTime);
	void SetHUDTime();
	void SyncTimeWithServer(float deltaTime);

	UFUNCTION(BlueprintCallable, Category = "Info" )
	void DoVoteToBounty(int32 bountyIndex);
	
	void AddVoteToBounty(int32 bountyIndex);
	void RemoveVoteFromBounty(int32 bountyIndex);

	bool LocalAddVoteToBounty (int32 bountyIndex);
	bool LocalRemoveVoteFromBounty (int32 bountyIndex);

	UFUNCTION( Server, Reliable )
	void ServerRemoveVoteFromBounty(int32 bountyIndex);
	UFUNCTION( Server, Reliable )
	void ServerAddVoteToBounty(int32 bountyIndex);
	UFUNCTION( Client, Reliable )
	void ClientVoteToBounty(bool  bSuccess, int32 bountyIndex);
	UFUNCTION( Server, Reliable )
	void ServerClearVote();

	UFUNCTION( Server, Reliable )
	void SeverAddMoney(int32 amount);
	UFUNCTION( Server, Reliable )
	void SeverRemoveMoney(int32 amount);
	
	void LocalAddMoney(int32 amount);
	void LocalRemoveMoney(int32 amount);

	void LoadGameSessionData();
	UFUNCTION( Server, Reliable )
	void SeverLoadGameSessionData();
	
	UFUNCTION( Client, Reliable )
	void ClientLoadGameSessionData(FGameSessionData GameSessionData);
	
	UFUNCTION( Server, Reliable )
	void ServerRequestTime(float timeOfClientRequest); // Request server time

	UFUNCTION( Client, Reliable )
	void ClientReportServerTime(float timeOfClientRequest, float serverTime); // Report the current server time to the client in response to a request
	
	UFUNCTION()
	void OnRep_MatchState();

	UFUNCTION()
	void OnRep_PlayerName();

	void OnMatchStateChanged();
	void HandleMatchStarted();
	void HandleMatchCooldown();
	void HandleMatchEnded();
	void DropAllItems();
	
	FString ConvertToTime(float time);

public:

	FHighPingDelegate HighPingDelegate;

	UPROPERTY(BlueprintAssignable)
	FVoteChangedDelegate VoteChangedDelegate;
	
	APW_PlayerController();
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void ReceivedPlayer() override;
	virtual bool ProcessConsoleExec(const TCHAR* Command, FOutputDevice& OutputDevice, UObject* Executor) override;
	
	void Destroyed() override;

	void DisplayAccouncement(const FString& message,FColor color = FColor::White, float duration = 2.0f);
	void HideAccouncement();
	void ToggleHUDVisibility(bool bShow);
	void SetHUDHealth(float health, float maxHealth);
	void SetHUDScore(float score);
	void SetMatchCountdown(float time);
	void SetMatchEndCountdown(float time);

	void OnMatchStateSet(FName matchState);

	UFUNCTION( Client, Reliable )
	void ClientOnLoadedInGameMode();

	void SetNewPlayerName();
	UFUNCTION( Server, Reliable )
	void ServerSetPlayerName();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetPlayerName(const FString& playerName);
	UFUNCTION( Client, Reliable )
	void ClientSetPlayerName(const FString& playerName);
	
	void TogglePlayerInput(bool bEnable);
	
	UFUNCTION( Client, Reliable )
	void ClientTogglePlayerInput(bool bEnable);

	UFUNCTION( Client, Reliable )
	void ClientToggleGravity(bool bEnable);

	UFUNCTION( Client, Reliable )
	void ClientActivateTrapMode(AActor* trap);

	UFUNCTION( Client, Reliable )
	void ClientDeactivateTrapMode();

	virtual float GetServerTime(); // Synced with server world clock

	UFUNCTION( Server, Reliable )
	void ServerCheckMatchState();

	UFUNCTION( Client, Reliable )
	void ClientJoinMidGame(FName stateOfMatch, float matchTime, float levelStartTime, float endMatchCountdown);

	void SpectatePlayer (APW_PlayerController* playerController);

	UFUNCTION( Client, Reliable )
	void ClientMoneyValueChanged(int32 money);

	UFUNCTION( Client, Reliable )
	void ClientShowAnnocement(const FString& message,FColor color, float duration);

	UFUNCTION( Client, Reliable )
	void ClientDayChanged(int32 day);
	
	UFUNCTION(BlueprintCallable)
	void AddMoney(int32 amount);
	UFUNCTION(BlueprintCallable)
	void RemoveMoney(int32 amount);

	void CollectCurrency(class APW_Currency* currency);
	UFUNCTION( Server, Reliable )
	void ServerCollectCurrency(class APW_Currency* currency);
	void LocalCollectCurrency(class APW_Currency* currency);

	bool IsAlive();
	
	void PayDebtCollector();
	UFUNCTION( Server, Reliable )
	void ServerPayDebtCollector();
	void LocalPayDebtCollector();

	void SpawnAutoEnemySpawner();
	
	UFUNCTION( Server, Reliable )
	void ServerSpawnAutoEnemySpawner(class APW_Character* controlledCharacter);
	void LocalSpawnAutoEnemySpawner(APW_Character* controlledCharacter);
	
	void LoadInventoryItemsByID (const TArray<FString>& itemIDs);

	TArray<FString> GetInventoryItemIDs();

	FORCEINLINE FString GetPlayerName() const { return _playerName; }
};
