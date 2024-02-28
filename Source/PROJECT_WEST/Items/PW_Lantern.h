// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PW_Item.h"
#include "PW_Lantern.generated.h"

UCLASS()
class PROJECT_WEST_API APW_Lantern : public APW_Item
{
	GENERATED_BODY()
	
public:	

	APW_Lantern();

protected:

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void OnVisibilityChange(bool bIsVisible) override;

	UFUNCTION()
	void OnBodyDetectionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
public:	

	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY (VisibleAnywhere, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* _lightBeamMesh;

	UPROPERTY (VisibleAnywhere , Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	class UPointLightComponent* _pointLight;

	UPROPERTY (VisibleAnywhere , Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* _bodyDetectionBox;
	
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

	void OnItemStateSet() override;
	
	void HandleTargetDetection(float DeltaTime);
	void HandleLightIntensity(float normalisedAngle);
	void HandleLightBeamScale(float normalisedAngle);

	void HandleDrainFuel(float amount);

	UFUNCTION(BlueprintCallable)
	void AddFuel(float amount);

	UFUNCTION(BlueprintCallable)
	void ReduceFuel(float amount);
	
	UFUNCTION( Server, Reliable )
	void ServerModifyFuel(float amount);
	void LocalModifyFuel(float amount);

	void DIssolveEnemy(AActor* enemy);
	UFUNCTION( Server, Reliable )
	void ServerDissolveEnemy(AActor* enemy);
	void LocalDissolveEnemy(AActor* enemy);

	void ToggleLightVisibility(bool visible);

	FORCEINLINE void SetTarget(AActor* target) { _target = target; }

	UFUNCTION(BlueprintCallable)
	float GetNormalisedFuel();
};
