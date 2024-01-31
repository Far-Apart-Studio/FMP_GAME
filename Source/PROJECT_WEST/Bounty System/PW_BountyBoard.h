// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PW_BountyBoard.generated.h"

UCLASS()
class PROJECT_WEST_API APW_BountyBoard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APW_BountyBoard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
				class UPrimitiveComponent* OtherComp, int32 OtherBodyIndexType, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
						class UPrimitiveComponent* OtherComp, int32 OtherBodyIndexType);

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BountyBoard", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* _root;
	
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BountyBoard", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* _bountyBoardMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BountyBoard", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* _bountyBoardWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BountyBoard", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* _triggerBox;

	void DetectOverlap();

	bool _isOverlapping = false;
};
