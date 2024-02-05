// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PW_PlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bPingTooHigh);

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

	FHighPingDelegate HighPingDelegate;

protected:
	
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	void StartHighPingWarning();
	void StopHighPingWarning();
	void HandleCheckPing(float DeltaTime);


	void SetHUDTime();
	void SyncTimeWithServer(float deltaTime);

	UFUNCTION( Server, Reliable )
	void ServerRequestTime(float timeOfClientRequest); // Request server time

	UFUNCTION( Client, Reliable )
	void ClientReportServerTime(float timeOfClientRequest, float serverTime); // Report the current server time to the client in response to a request

	float _clientServerDelta; // Difference between client and server time

	UPROPERTY(EditAnywhere, Category = "Time", meta = (AllowPrivateAccess = "true"))
	float _timeSyncFrequency; // How often to sync client and server time

	UPROPERTY(VisibleAnywhere, Category = "Time", meta = (AllowPrivateAccess = "true"))
	float _timeSyncRuningTime;

	void PawnLeavingGame() override;
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
	
	uint32 _countDownInt;

	class APW_BountyGameMode* _bountyGameMode;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName _matchState;

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
