#pragma once
#include "CoreMinimal.h"
#include "InfoData.generated.h"

UENUM(BlueprintType)
enum class EInfoType : uint8
{
	EBounty,
	EWeapons,
	EBountyBoard,
	ELantern,
	ESideObjective,
	EExtraction,
};

USTRUCT(BlueprintType)
struct FInfoEntry
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data)
	EInfoType _infoType;
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data)
	FString _name;
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data)
	FString _usage;
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data)
	FString _description;
};