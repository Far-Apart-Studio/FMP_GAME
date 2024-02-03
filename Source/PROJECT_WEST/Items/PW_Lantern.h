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
	// Sets default values for this actor's properties
	APW_Lantern();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void OnVisibilityChange(bool bIsVisible) override; 
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* _lightBeamMesh;

	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	class UPointLightComponent* _pointLight;

	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	AActor* _target;

	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	float _maxLightIntensity;

	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	float _minLightIntensity;

	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	float _currentLightIntensity;

	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	float _minBeamScale;

	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	float _maxBeamScale;

	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	float _currentBeamScale;

	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	float _maxSearchDistance;

	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	float _minSearchDistance;

	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	float _currentSearchDistance;

	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	float _maxFuel;
	
	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	float _currentFuel;

	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Lantern" , meta = (AllowPrivateAccess = "true"))
	float _fuelDrainRate;
	
public:

	void OnItemStateSet() override;
	
	void HandleTargetDetection(float DeltaTime);
	void HandleLightIntensity(float normalisedAngle);
	void HandleLightBeamScale(float normalisedAngle);

	void ChargeFuel(float amount);
	void HandleDrainFuel(float DeltaTime);

	void ToggleLightVisibility(bool visible);

	FORCEINLINE void SetTarget(AActor* target) { _target = target; }
	FORCEINLINE float GetNormalisedFuel() const { return _currentFuel / _maxFuel; }
};
