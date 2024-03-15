// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PROJECT_WEST/SideObjective/SideObjective.h"
#include "PW_SideObjective.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveCompleted, APW_SideObjective*, complectedObjective);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveFailed, APW_SideObjective*, failedObjective);

UCLASS()
class PROJECT_WEST_API APW_SideObjective : public AActor
{
	GENERATED_BODY()

protected:
	
	virtual void BeginPlay() override;
	void HandleTimer(float deltaTime);
	
public:	

	APW_SideObjective();

	virtual void Tick(float DeltaTime) override;

	virtual void SetUp(FSideObjectiveEntry sideObjectiveData, class APW_PoiArea* poiArea);
	virtual void Deactivate();

	UFUNCTION()
	void OnPOITriggered(class APW_PoiArea* Poi);

	FString GetElapsedTime();
	FString ConvertToTime(float time);

	UPROPERTY(BlueprintAssignable, Category = "SideObjective")
	FOnObjectiveCompleted _onObjectiveCompleted;

	UPROPERTY(BlueprintAssignable, Category = "SideObjective")
	FOnObjectiveFailed _onObjectiveFailed;

	FORCEINLINE FSideObjectiveEntry GetObjectiveData() const { return _objectiveData; }
	FORCEINLINE APW_PoiArea* GetPoiArea() const { return _poiArea; }

protected:

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	TArray< AActor*> _targetActors;
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	FSideObjectiveEntry _objectiveData;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	class APW_PoiArea* _poiArea;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	float _currentObjectiveTime;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	bool _startTimer;
};
