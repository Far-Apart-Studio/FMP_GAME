// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW_ItemObject.h"
#include "GameFramework/Actor.h"
#include "PW_LanternObject.generated.h"

class UPointLightComponent;
class UBoxComponent;

UCLASS()
class PROJECT_WEST_API APW_LanternObject : public APW_ItemObject
{
	GENERATED_BODY()

private:
	UPROPERTY (VisibleAnywhere, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* _lightBeamMesh;

	UPROPERTY (VisibleAnywhere , Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	UPointLightComponent* _pointLight;

	UPROPERTY (VisibleAnywhere , Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	UBoxComponent* _bodyDetectionBox;
	
	UPROPERTY (EditAnywhere, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	float _maxLightIntensity;

	UPROPERTY (EditAnywhere, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	float _minLightIntensity;

	UPROPERTY (VisibleAnywhere, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	float _currentLightIntensity;

	UPROPERTY (EditAnywhere, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	float _minBeamScale;

	UPROPERTY (EditAnywhere, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	float _maxBeamScale;

	UPROPERTY (VisibleAnywhere, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	float _currentBeamScale;

	UPROPERTY (EditAnywhere, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	float _maxFuel;

	UPROPERTY (EditAnywhere, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	float _fuelPerCharge;
	
	UPROPERTY (EditAnywhere, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	float _fuelDrainRate;
	
	UPROPERTY (Replicated, EditAnywhere, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	AActor* _target;
	
	UPROPERTY (Replicated, EditAnywhere, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	float _currentFuel;
	

public:	
	APW_LanternObject();

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void EnterHeldState() override;
	virtual void EnterDroppedState() override;

	UFUNCTION()
	void OnBodyDetectionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	void HandleTargetDetection(float DeltaTime);
	void HandleLightIntensity(float normalisedAngle);
	void HandleLightBeamScale(float normalisedAngle);
	void HandleDrainFuel(float amount);
	void LocalModifyFuel(float amount);
	void DissolveEnemy(AActor* enemy);
	void LocalDissolveEnemy(AActor* enemy);
	void ToggleLightVisibility(bool visible);

	UFUNCTION(BlueprintCallable)
	void AddFuel(float amount);
	
	UFUNCTION(BlueprintCallable)
	void ReduceFuel(float amount);
	
	UFUNCTION( Server, Reliable )
	void ServerModifyFuel(float amount);

	UFUNCTION( Server, Reliable )
	void ServerDissolveEnemy(AActor* enemy);
	
	UFUNCTION(BlueprintCallable)
	float GetNormalisedFuel();
	
	FORCEINLINE void SetTarget(AActor* target) { _target = target; }
	
};
