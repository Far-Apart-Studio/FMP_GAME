// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PW_PlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bPingTooHigh);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FVoteChangedDelegate, bool, bsuccess, int32, bountyIndex);
/**
 * 
 */
UCLASS()
class PROJECT_WEST_API APW_PlayerController : public APlayerController
{
	GENERATED_BODY()


public:
	
	APW_PlayerController();
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void ReceivedPlayer() override; // Sync with server clock as soon as possible
	
	void Destroyed() override;

	FHighPingDelegate HighPingDelegate;

	UPROPERTY(BlueprintAssignable)
	FVoteChangedDelegate VoteChangedDelegate;

protected:
	
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
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

	UFUNCTION(BlueprintCallable)
	void AddMoney(int32 amount);
	UFUNCTION(BlueprintCallable)
	void RemoveMoney(int32 amount);

	UFUNCTION( Server, Reliable )
	void SeverAddMoney(int32 amount);
	UFUNCTION( Server, Reliable )
	void SeverRemoveMoney(int32 amount);
	
	void LocalAddMoney(int32 amount);
	void LocalRemoveMoney(int32 amount);
	
	UFUNCTION( Server, Reliable )
	void ServerRequestTime(float timeOfClientRequest); // Request server time

	UFUNCTION( Client, Reliable )
	void ClientReportServerTime(float timeOfClientRequest, float serverTime); // Report the current server time to the client in response to a request

	float _clientServerDelta; // Difference between client and server time

	UPROPERTY(EditAnywhere, Category = "Time", meta = (AllowPrivateAccess = "true"))
	float _timeSyncFrequency; // How often to sync client and server time

	UPROPERTY(VisibleAnywhere, Category = "Time", meta = (AllowPrivateAccess = "true"))
	float _timeSyncRuningTime;
	
public:

	void SetHUDHealth(float health, float maxHealth);
	void SetHUDScore(float score);
	void SetMatchCountdown(float time);
	void SetMatchEndCountdown(float time);

	void OnMatchStateSet(FName matchState);
	
	void TogglePlayerInput(bool bEnable);
	
	UFUNCTION( Client, Reliable )
	void ClientTogglePlayerInput(bool bEnable);

	virtual float GetServerTime(); // Synced with server world clock

	UFUNCTION( Server, Reliable )
	void ServerCheckMatchState();

	UFUNCTION( Client, Reliable )
	void ClientJoinMidGame(FName stateOfMatch, float matchTime, float levelStartTime, float endMatchCountdown);

	void SpectatePlayer (APW_PlayerController* playerController);

	UFUNCTION( Client, Reliable )
	void ClientMoneyValueChanged(int32 money);

	bool IsAlive();
	
private:
	
	class APW_HUD* _hud;

	// To show a warning when ping is too high
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess = "true"))
	float _money;
	
	uint32 _countDownInt;

	bool _hasVoted;
	int32 _votedBountyIndex;

	class APW_BountyGameMode* _bountyGameMode;
	class APW_LobbyGameMode* _lobbyGameMode;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName _matchState;

	FString _matchTimeString;

	UFUNCTION()
	void OnRep_MatchState();

	void OnMatchStateChanged();
	void HandleMatchStarted();
	void HandleMatchCooldown();
	void HandleMatchEnded();
	void DropAllItems();

	bool IsHUDValid();
	FString ConvertToTime(float time);
};
