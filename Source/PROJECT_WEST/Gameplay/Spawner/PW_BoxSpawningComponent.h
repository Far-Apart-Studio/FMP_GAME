// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PW_BoxSpawningComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_BoxSpawningComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	UPROPERTY(VisibleAnywhere, Category = "Gameplay",meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* _boxComponent;
	
	UPROPERTY(EditAnywhere, Category = "Gameplay",meta = (AllowPrivateAccess = "true"))
	float _spawnHeightOffset;

public:	

	UPW_BoxSpawningComponent();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	bool GetGroundPositionAndNormal(FVector origin, FVector& outPosition);
	
	FVector GetRandomPositionInBox();

	FVector GetRandomPositionInBoxEdge();

	FORCEINLINE void SetBoxComponent(UBoxComponent* boxComponent) { _boxComponent = boxComponent; }
};
