// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_Lantern.h"
#include "Components/PointLightComponent.h"
#include "DebugMacros.h"

// Sets default values
APW_Lantern::APW_Lantern()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	_lightBeamMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightBeamMesh"));
	_lightBeamMesh->SetupAttachment(_mesh);
	
	_pointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	_pointLight->SetupAttachment(_mesh);

	_currentLightIntensity = _minLightIntensity = 1000.0f;
	_maxLightIntensity = 10000.0f;

	_currentBeamScale = _minBeamScale = 1.0f;
	_maxBeamScale = 10.0f;
}

// Called when the game starts or when spawned
void APW_Lantern::BeginPlay()
{
	Super::BeginPlay();
	_pointLight->SetIntensity(_currentLightIntensity);
	_lightBeamMesh->SetRelativeScale3D(FVector(_currentBeamScale, _currentBeamScale, 1.0f));
}

// Called every frame
void APW_Lantern::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	HandleTargetDetection (DeltaTime);
}

void APW_Lantern::HandleTargetDetection(float DeltaTime)
{
	// Use the foward vector og the player instead of the lantern
	const float dotProduct = FVector::DotProduct (GetActorForwardVector (), _target->GetActorForwardVector ());
	const float angle = FMath::RadiansToDegrees (FMath::Acos (dotProduct));
	const float normalisedAngle =  angle / 180.0f;

	HandleLightIntensity (normalisedAngle);
	HandleLightBeamScale (normalisedAngle);
}

void APW_Lantern::HandleLightIntensity(float normalisedAngle)
{
	_currentLightIntensity = FMath::Lerp (_minLightIntensity, _maxLightIntensity, normalisedAngle);
	_pointLight -> SetIntensity (1 - _currentLightIntensity);
}

void APW_Lantern::HandleLightBeamScale(float normalisedAngle)
{
	_currentBeamScale = FMath::Lerp (_minBeamScale, _maxBeamScale, normalisedAngle);
	_lightBeamMesh -> SetRelativeScale3D (FVector (_currentBeamScale, _currentBeamScale, 1.0f));
}

