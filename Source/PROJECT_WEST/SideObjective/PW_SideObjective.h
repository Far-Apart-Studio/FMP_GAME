// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PROJECT_WEST/SideObjective/SideObjective.h"
#include "PW_SideObjective.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveStatChanged, APW_SideObjective*, objective);

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
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	virtual void SetUp(FSideObjectiveEntry sideObjectiveData, class APW_PoiArea* poiArea);
	virtual void Completed();
	virtual void Failed();

	UFUNCTION()
	void OnPOITriggered(class APW_PoiArea* Poi);

	UFUNCTION()
	void OnRep_Complected();

	FString GetElapsedTime();
	FString ConvertToTime(float time);

	UPROPERTY(BlueprintAssignable, Category = "SideObjective")
	FOnObjectiveStatChanged _onObjectiveStatChanged;

	FORCEINLINE FSideObjectiveEntry GetObjectiveData() const { return _objectiveData; }
	FORCEINLINE APW_PoiArea* GetPoiArea() const { return _poiArea; }
	FORCEINLINE EObjectiveState GetObjectiveState() const { return _objectiveState; }

protected:

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	TArray< AActor*> _targetActors;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	FSideObjectiveEntry _objectiveData;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	class APW_PoiArea* _poiArea;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	float _currentObjectiveAmount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	float _currentObjectiveTime;
	
	UPROPERTY(Replicated, Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
    FString _timeText;

	UPROPERTY( ReplicatedUsing=OnRep_Complected, VisibleAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	EObjectiveState _objectiveState;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	bool _startTimer;
};
