// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PW_BountyData.h"
#include "PROJECT_WEST/Interfaces//PW_InteractableInterface.h"
#include "PW_BountyBoard.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBountyDataListChanged, const TArray<FBountyDataEntry>&, bountyDataList);

UCLASS()
class PROJECT_WEST_API APW_BountyBoard : public AActor, public IPW_InteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APW_BountyBoard();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void StartFocus_Implementation() override;
	void EndFocus_Implementation() override;
	void StartInteract_Implementation(AActor* owner) override;
	void EndInteract_Implementation() override;
	bool IsInteracting_Implementation() override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BountyBoard", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* _root;
	
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BountyBoard", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* _bountyBoardMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BountyBoard", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* _bountyBoardWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BountyBoard", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* _cameraPosition;

	UPROPERTY(ReplicatedUsing = OnRep_BountyListChanged, BlueprintReadWrite, Category = "BountyBoard", meta = (AllowPrivateAccess = "true"))
	TArray<FBountyDataEntry> _bountyDataList;

	UPROPERTY(BlueprintAssignable, Category = "BountyBoard", meta = (AllowPrivateAccess = "true"))
	FBountyDataListChanged _bountyDataListChanged;

	class APW_Character* _character;

	UFUNCTION()
	void OnRep_BountyListChanged();

	UFUNCTION( BlueprintCallable, Category = "BountyBoard" )
	void PopulateBountyDataList();

	bool _isOverlapping = false;

	void FocusCharacterCameraOnBountyBoard(class APW_Character* character);
};
