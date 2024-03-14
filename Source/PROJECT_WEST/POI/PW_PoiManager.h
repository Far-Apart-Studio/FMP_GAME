// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PW_PoiManager.generated.h"

UCLASS()
class PROJECT_WEST_API APW_PoiManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APW_PoiManager();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual void BeginPlay() override;

	void GetAllChildPoIActors();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	TArray<class APW_PoiArea*> _poiAreas;

public:	

	UFUNCTION(BlueprintCallable, Category = Data)
	TArray<class APW_PoiArea*> GetPoiAreas() const { return _poiAreas; }

};
