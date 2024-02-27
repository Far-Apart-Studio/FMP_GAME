// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PW_ActorMoverComponent.generated.h"

UENUM()
enum class EMoveDirection: uint8
{
	EMD_Forward UMETA(DisplayName = "Foward"),
	EMD_Backward UMETA(DisplayName = "Backward")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_ActorMoverComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UPW_ActorMoverComponent();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetPoints(FVector point1, FVector point2);
	FORCEINLINE void SetCanMove(bool canMove) { _canMove = canMove; }
	FORCEINLINE void SetMoveDirection(EMoveDirection moveDirection) { _moveDirection = moveDirection; }
	
private:
	
	UPROPERTY(VisibleAnywhere, Category = "SpawnPoints")
	FVector _startPoint;

	UPROPERTY(VisibleAnywhere, Category = "SpawnPoints")
	FVector _endPoint;

	UPROPERTY(EditAnywhere, Category = "SpawnPoints")
	float _moveTime;

	bool _canMove;

	EMoveDirection _moveDirection;
	
	void HandleMovement(float DeltaTime);
};
