// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PW_BountyData.h"
#include "BountySystemComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UBountySystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UBountySystemComponent();


protected:

	virtual void BeginPlay() override;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Bounty System" )
	FBountyMapData _bountyMapData;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Bounty System" )
	FBountyDifficultyData _bountyDifficultyData;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Bounty System" )
	FBountyTargets _bountyTargets;

	UFUNCTION(BlueprintCallable, Category = "Bounty System" )
	FBountyDataEntry GetBountyDataEntry(EBountyDifficulty _bountyDifficulty);

	UFUNCTION(BlueprintCallable, Category = "Bounty System" )
	TArray<FBountyDataEntry> GetBountyDataList(TArray<EBountyDifficulty> _bountyDifficulties);
};
