// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PROJECT_WEST/Notification.h"
#include "PROJECT_WEST\InfoData.h"
#include "GameFramework/GameMode.h"
#include "PW_GameMode.generated.h"


struct FGameSessionData;
struct FInfoEntry;

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API APW_GameMode : public AGameMode
{
	GENERATED_BODY()

	
protected:

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "GameMode" , meta = (AllowPrivateAccess = "true") )
	class UPW_GameInstance* _gameInstance = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameMode", meta = (AllowPrivateAccess = "true"))
	UDataTable* _mapDataTable;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "GameMode" , meta = (AllowPrivateAccess = "true") )
	TArray<int32> _colorIndexes;

protected:

	APW_GameMode();
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	virtual void Logout(AController* Exiting) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	virtual void OnMatchStateSet() override;

	virtual void LoadGameSessionData();
	
	virtual void PlayerEliminated(class APW_Character* ElimmedCharacter, class APW_PlayerController* VictimController, AController* AttackerController);

	void ToggleAllPlayersInput(bool bEnable);

	void ToggleSessionLock(bool lock);

	TSubclassOf<class APW_ItemObject> GetItemObjectFromDataTable(FString id);
	
	int32 GetRandomColorIndex();
	
public:

	void PlayerLeftGame(class APW_PlayerState* PlayerLeaving);

	UFUNCTION(BlueprintCallable)
	int32 GetNumPlayerInSession() const;

	UFUNCTION(BlueprintCallable)
	void ServerTravel (FString mapPath);

	UFUNCTION(BlueprintCallable)
	void ServerTravelMapEnty (FMapDataEntry mapdata);
	
	void AddMoney(int32 amount);
	void RemoveMoney (int32 amount);
	void NotifyPlayersOfMoney();
	int32 GetMoney() const;

	void SetDay(int day);
	void NofigyPlayersOfDay();

	void SaveAllPlayersInventoryData();
	void SavePlayerInventoryData( class APW_PlayerController* playerController) const;

	void CollectCurrency(class APW_Currency* Currency);;

	void TriggerPlayersAnnouncement(const FString& announcement,FColor color, float duration) const;

	void TriggerPlayersLoadingScreen(const FMapDataEntry& level);

	void TriggerNotification(const FNotificationEntry& notification) const;
	void TriggerTutorialNotification(const FInfoEntry& info) const;

	FString GetPlayerName(APlayerController* playerController) const;

	FGameSessionData& GetGameSessionData();

	FORCEINLINE class UPW_GameInstance* GetCurrentGameInstance() const { return _gameInstance; }
};
