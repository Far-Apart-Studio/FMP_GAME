// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PW_BountyData.h"
#include "PROJECT_WEST/Interfaces//PW_InteractableInterface.h"
#include "PW_BountyBoard.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBountyDataListChanged, const TArray<FBountyDataEntry>&, bountyDataList);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBountyVoteDataChanged, const FBountyVoteData&, bountyVoteData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE (FOnBoardOpened);
DECLARE_DYNAMIC_MULTICAST_DELEGATE (FOnBoardClosed);

UCLASS()
class PROJECT_WEST_API APW_BountyBoard : public AActor, public IPW_InteractableInterface
{
	GENERATED_BODY()
	
public:	
	APW_BountyBoard();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void StartFocus_Implementation() override;
	virtual void EndFocus_Implementation() override;
	virtual void StartInteract_Implementation(AActor* owner) override;
	virtual void EndInteract_Implementation() override;
	virtual bool IsInteracting_Implementation() override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* _root;
	
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* _bountyBoardMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* _bountyBoardWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* _cameraPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* _cameraComponent;

	UPROPERTY(ReplicatedUsing = OnRep_BountyListChanged, BlueprintReadWrite, Category = "Info", meta = (AllowPrivateAccess = "true"))
	TArray<FBountyDataEntry> _bountyDataList;

	UPROPERTY(BlueprintAssignable, Category = "Info", meta = (AllowPrivateAccess = "true"))
	FBountyDataListChanged _bountyDataListChanged;

	UPROPERTY(BlueprintAssignable, Category = "Info", meta = (AllowPrivateAccess = "true"))
	FBountyVoteDataChanged _bountyVoteDataChanged;

	UPROPERTY(BlueprintAssignable, Category = "Info", meta = (AllowPrivateAccess = "true"))
	FOnBoardOpened _onBoardOpened;

	UPROPERTY(BlueprintAssignable, Category = "Info", meta = (AllowPrivateAccess = "true"))
	FOnBoardClosed _onBoardClosed;
	
	class APW_Character* _character;

	UFUNCTION()
	void OnRep_BountyListChanged();

	UPROPERTY(ReplicatedUsing = OnRep_BountyVoteChanged, BlueprintReadWrite, Category = "Info", meta = (AllowPrivateAccess = "true"))
	FBountyVoteData _bountyVoteData;

	UFUNCTION()
	void OnRep_BountyVoteChanged();

public:	

	UFUNCTION( BlueprintCallable, Category = "Info" )
	void PopulateBountyDataList();
	
	void PopulateBountyVoteData(int numberOfBounties);

	UFUNCTION( BlueprintCallable, Category = "Info" )
	int32 GetTotalVotes();
	
	UFUNCTION( BlueprintCallable, Category = "Info" )
	int32 GetBountyIndexWithHighestVotes();

	FBountyDataEntry GetBountyWithHighestVotes();

	void ToggleHighlight(bool status);

	UFUNCTION(BlueprintCallable, Category = "Info" )
	void AddVoteToBounty(int32 bountyIndex);
	
	UFUNCTION( BlueprintCallable, Category = "Info" )
	void RemoveVoteFromBounty(int32 bountyIndex);

	FORCEINLINE TArray<FBountyDataEntry> GetBountyDataList() const { return _bountyDataList; }
};
