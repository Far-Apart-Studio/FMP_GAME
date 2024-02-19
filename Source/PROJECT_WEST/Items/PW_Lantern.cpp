// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_Lantern.h"
#include "Components/PointLightComponent.h"
#include "Components/BoxComponent.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "Net/UnrealNetwork.h"
#include "PROJECT_WEST/PW_HealthComponent.h"
#include "PROJECT_WEST/PW_Character.h"

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

	_bodyDetectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyDetectionBox"));
	_bodyDetectionBox->SetupAttachment(_itemMesh);
	_bodyDetectionBox->SetIsReplicated(true);
	_bodyDetectionBox->SetCollisionProfileName("Trigger");

	_currentLightIntensity = _minLightIntensity = 1000.0f;
	_maxLightIntensity = 10000.0f;

	_currentBeamScale = _minBeamScale = .5f;
	_maxBeamScale = 2.0f;

	_maxSearchDistance = 1000.0f;
	_minSearchDistance = 100.0f;
	
	_maxFuel = 100.0f;
	_fuelPerCharge = 10.0f;
	_fuelDrainRate = 10.0f;
}

// Called when the game starts or when spawned
void APW_Lantern::BeginPlay()
{
	Super::BeginPlay();
	_pointLight->SetIntensity(_currentLightIntensity);
	_lightBeamMesh->SetRelativeScale3D(FVector(_currentBeamScale, _currentBeamScale, 1.0f));
	ToggleLightVisibility(false);
	_currentFuel = 0.0f;

	if (HasAuthority())
	{
		_bodyDetectionBox->OnComponentBeginOverlap.AddDynamic(this, &APW_Lantern::OnBodyDetectionBoxBeginOverlap);
	}
}

void APW_Lantern::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APW_Lantern, _target);
	DOREPLIFETIME(APW_Lantern, _currentFuel);
}

void APW_Lantern::OnVisibilityChange(bool bIsVisible)
{
	Super::OnVisibilityChange(bIsVisible);
	_lightBeamMesh->SetVisibility(bIsVisible);
	_pointLight->SetVisibility(bIsVisible);
}

void APW_Lantern::OnBodyDetectionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!_target  || !_isVisible || _itemState == EItemState::EIS_Dropped) return;
	if (OtherActor == nullptr || OtherActor == this) return;
	if(APW_Character* character = Cast<APW_Character>(OtherActor)) return;
	
	UPW_HealthComponent* healthComponent = OtherActor->FindComponentByClass<UPW_HealthComponent>();
	if (healthComponent && !healthComponent->IsAlive())
	{
		AddFuel();
		OtherActor->Destroy();
	}
}

// Called every frame
void APW_Lantern::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	HandleTargetDetection (DeltaTime);

	if (HasAuthority())
	{
		HandleDrainFuel(DeltaTime);
	}
}

void APW_Lantern::OnItemStateSet()
{
	Super::OnItemStateSet();
	ToggleLightVisibility(_isVisible && _itemState == EItemState::EIS_Pickup);
}

void APW_Lantern::HandleTargetDetection(float DeltaTime)
{
	if(!_target  || !_isVisible || _itemState == EItemState::EIS_Dropped) return;

	_currentSearchDistance = FMath::Lerp (_minSearchDistance, _maxSearchDistance, _currentFuel / _maxFuel);

	const float distance = FVector::Dist (GetActorLocation (), _target->GetActorLocation ());

	bool inRange = distance < _currentSearchDistance;

	ToggleLightVisibility (inRange);
	
	if (!inRange)
	{
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
	_lightBeamMesh -> SetRelativeScale3D (FVector (_currentBeamScale, _currentBeamScale, _lightBeamMesh -> GetRelativeScale3D ().Z));
}

void APW_Lantern::AddFuel()
{
	ChargeFuel(_fuelPerCharge);
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
	LocalReduceFuel(_fuelDrainRate * DeltaTime);
}

void APW_Lantern::ReduceFuel(float amount)
{
	if (HasAuthority())
	{
		LocalReduceFuel(amount);
	}
	else if (!_isInProgress)
	{
		_isInProgress = true;
		ServerReduceFuel(amount);
	}
}

void APW_Lantern::ServerReduceFuel_Implementation(float DeltaTime)
{
	if (!HasAuthority()) return;
	LocalReduceFuel(DeltaTime);
	_isInProgress = false;
}

void APW_Lantern::LocalReduceFuel(float amount)
{
	_currentFuel = FMath::Clamp(_currentFuel - amount, 0.0f, _maxFuel);
}

void APW_Lantern::ToggleLightVisibility(bool visible)
{
	_lightBeamMesh->SetVisibility(visible);
	_pointLight->SetVisibility(visible);
}

float APW_Lantern::GetNormalisedFuel()
{
	return _currentFuel / _maxFuel;
}
