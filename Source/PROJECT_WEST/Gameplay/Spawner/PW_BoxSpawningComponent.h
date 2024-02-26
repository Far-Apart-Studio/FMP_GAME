// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PW_BoxSpawningComponent.generated.h"

USTRUCT(BlueprintType)
struct FSpawnItemInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "SpawnItemInfo" , meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float _weight;
	
	UPROPERTY(EditAnywhere, Category = "SpawnItemInfo")
	TSubclassOf<AActor> itemClass;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_BoxSpawningComponent : public USceneComponent
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, Category = "Gameplay",meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* _boxComponent;

	UPROPERTY(EditAnywhere, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> testActorClass;

public:	
	// Sets default values for this component's properties
	UPW_BoxSpawningComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	AActor* SpawnActorInBox(TSubclassOf<AActor> actorClass);
	
private:
	
	FVector GetRandomPositionInBox();
	bool IsPositionValid(FVector position);
	void GetGroundPositionAndNormal(FVector& outPosition, FVector& outNormal);
};
