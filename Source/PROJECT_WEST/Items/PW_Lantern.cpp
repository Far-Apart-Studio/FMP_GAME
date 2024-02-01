// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_Lantern.h"
#include "Components/PointLightComponent.h"
#include "PROJECT_WEST/DebugMacros.h"

// Sets default values
APW_Lantern::APW_Lantern()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
	SetReplicateMovement(true);
	
	_lightBeamMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightBeamMesh"));
	_lightBeamMesh->SetupAttachment(_itemMesh);
	_lightBeamMesh->SetIsReplicated(true);
	
	_pointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	_pointLight->SetupAttachment(_itemMesh);
	_pointLight->SetIsReplicated(true);

	_currentLightIntensity = _minLightIntensity = 1000.0f;
	_maxLightIntensity = 10000.0f;

	_currentBeamScale = _minBeamScale = .5f;
	_maxBeamScale = 2.0f;

	_maxSearchDistance = 1000.0f;
	_minSearchDistance = 100.0f;
	
	_maxFuel = 100.0f;

	// TODO: Remove this
	_currentFuel = _maxFuel;
	_fuelDrainRate = 10.0f;
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
	//HandleDrainFuel (DeltaTime);
	HandleTargetDetection (DeltaTime);
}

void APW_Lantern::HandleTargetDetection(float DeltaTime)
{
	if(!_target  || _itemState != EItemState::EIS_Equipped) return;

	_currentSearchDistance = FMath::Lerp (_minSearchDistance, _maxSearchDistance, _currentFuel / _maxFuel);

	const float distance = FVector::Dist (GetActorLocation (), _target->GetActorLocation ());
	
	if (distance > _currentSearchDistance)
	{
		HandleLightIntensity (0.0f);
		HandleLightBeamScale (1);
		return;
	}
	
	AActor* player = GetOwner ();
	if (!player) return;
	
	//const float dotProduct = FVector::DotProduct (player->GetActorForwardVector (), _target->GetActorForwardVector ());
	
	FVector playerForward = player->GetActorForwardVector();
	FVector toTarget = _target->GetActorLocation() - player->GetActorLocation();
	playerForward.Normalize();
	toTarget.Normalize();
	const float dotProduct = FVector::DotProduct (playerForward, toTarget);

	//DEBUG_STRING( "Dot Product: " + FString::SanitizeFloat(dotProduct) );
	
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

void APW_Lantern::ChargeFuel(float amount)
{
	_currentFuel += amount;
	if (_currentFuel > _maxFuel)
	{
		_currentFuel = _maxFuel;
	}
}

void APW_Lantern::HandleDrainFuel(float DeltaTime)
{
	if (_currentFuel <= 0.0f) return;
	_currentFuel -= _fuelDrainRate * DeltaTime;
}
