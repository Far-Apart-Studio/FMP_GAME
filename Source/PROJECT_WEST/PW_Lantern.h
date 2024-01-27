// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PW_Lantern.generated.h"

UCLASS()
class PROJECT_WEST_API APW_Lantern : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APW_Lantern();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
public:

	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = "Lantern" )
	class UStaticMeshComponent* _mesh;

	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = "Lantern" )
	class UStaticMeshComponent* _lightBeamMesh;

	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = "Lantern" )
	class UPointLightComponent* _pointLight;

	UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Lantern" )
	AActor* _target;

	UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Lantern" )
	float _maxLightIntensity;

	UPROPERTY ( EditAnywhere, BlueprintReadWrite, Category = "Lantern" )
	float _minLightIntensity;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Lantern" )
	float _currentLightIntensity;

	UPROPERTY ( EditAnywhere, BlueprintReadWrite, Category = "Lantern" )
	float _minBeamScale;

	UPROPERTY ( EditAnywhere, BlueprintReadWrite, Category = "Lantern" )
	float _maxBeamScale;

	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = "Lantern" )
	float _currentBeamScale;

	void HandleTargetDetection(float DeltaTime);
	void HandleLightIntensity(float normalisedAngle);
	void HandleLightBeamScale(float normalisedAngle);

	FORCEINLINE void SetTarget(AActor* target) { _target = target; }
};
