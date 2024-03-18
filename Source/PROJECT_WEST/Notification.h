#pragma once
#include "CoreMinimal.h"
#include "Notification.generated.h"

UENUM(BlueprintType)
enum class ENotificationType : uint8
{
	EDeath,
	ELocation,
	EElimination,
	EReward,
	EInfo
};

USTRUCT(BlueprintType)
struct FNotificationEntry
{
	GENERATED_BODY()
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data)
	ENotificationType _notificationType;
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data)
	FString _playerNameText;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data)
	FString _notificationText;

	
};
