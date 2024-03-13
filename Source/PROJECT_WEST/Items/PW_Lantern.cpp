// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_Lantern.h"
#include "Components/PointLightComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "Net/UnrealNetwork.h"
#include "PROJECT_WEST/PW_HealthComponent.h"
#include "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/Gameplay/Components/PW_MaterialEffectComponent.h"

APW_Lantern::APW_Lantern()
{
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

	_minCorrectAngle = 30.0f;
	_maxCorrectAngle = 60.0f;
	
	_maxFuel = 100.0f;
	_fuelPerCharge = 10.0f;
	_fuelDrainRate = 10.0f;

	_minOffsetToTarget = 500.0f;
	_maxOffsetToTarget = 1000.0f;
}

void APW_Lantern::BeginPlay()
{
	Super::BeginPlay();
	_pointLight->SetIntensity(_currentLightIntensity);
	_lightBeamMesh->SetRelativeScale3D(FVector(_currentBeamScale, _currentBeamScale, 1.0f));
	ToggleLightVisibility(false);
	_currentFuel = 100.0f;

	float random = FMath::RandRange(0, 1);
	_offsetToTarget = FMath::RandRange(_minOffsetToTarget, _maxOffsetToTarget);
	_offsetToTarget *= random >= 0.5 ? 1 : -1;

	_currentCorrectAngle = FMath::RandRange(_minCorrectAngle, _maxCorrectAngle);
	
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

void APW_Lantern::OnSetVisibility()
{
	Super::OnSetVisibility();
	_lightBeamMesh->SetVisibility(_isVisible);
	_pointLight->SetVisibility(_isVisible);
}

void APW_Lantern::OnBodyDetectionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!_target  || !_isVisible || _itemState == EItemObjectState::EDropped) return;
	if (OtherActor == nullptr || OtherActor == this) return;
	
	if(APW_Character* character = Cast<APW_Character>(OtherActor)) return;
	
	UPW_HealthComponent* healthComponent = OtherActor->FindComponentByClass<UPW_HealthComponent>();
	if (healthComponent && !healthComponent->IsAlive())
	{
		AddFuel(_fuelPerCharge);
		
		UCapsuleComponent* capsuleComponent = OtherActor->FindComponentByClass<UCapsuleComponent>();
		if (capsuleComponent)
		{
			capsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		DIssolveEnemy(OtherActor);
	}
}

void APW_Lantern::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	HandleTargetDetection (DeltaTime);

	if (HasAuthority())
	{
		HandleDrainFuel(DeltaTime);
	}

	//GetTargetLocation();
}

void APW_Lantern::OnUpdateItemState()
{
	Super::OnUpdateItemState();
	ToggleLightVisibility(_isVisible && _itemState == EItemObjectState::EHeld);
}

void APW_Lantern::HandleTargetDetection(float DeltaTime)
{
	if(!_target  || !_isVisible || _itemState == EItemObjectState::EDropped) return;
	
	AActor* player = GetOwner ();
	if (!player) return;
	
	FVector playerForward = player->GetActorForwardVector();

	const float currentOffsetToTarget = FMath::Lerp( _offsetToTarget, 0.0f, _currentFuel / _maxFuel);
	const FVector targetLocation = _target->GetActorLocation() + _target->GetActorRightVector() * currentOffsetToTarget;

	//DRAW_SPHERE_SingleFrame (targetLocation);
	
	FVector toTarget = targetLocation - player->GetActorLocation();
	playerForward.Normalize();
	toTarget.Normalize();
	
	const float dotProduct = FVector::DotProduct (playerForward, toTarget);
	const float angle = FMath::RadiansToDegrees (FMath::Acos (dotProduct));
	
	const float correctAngle = FMath::Lerp (_currentCorrectAngle, 0.0f, _currentFuel / _maxFuel);
	const float normalisedAngle = FMath::Clamp ((angle - correctAngle) / (180.0f - correctAngle), 0.0f, 1.0f);

	//DEBUG_STRING ( "Normalised Fuel: " + FString::SanitizeFloat (_currentFuel / _maxFuel) +
	//" - " + "Current Offset: " + FString::SanitizeFloat (currentOffsetToTarget) +
	//	" - " + "Normalised Angle: " + FString::SanitizeFloat (normalisedAngle) );
	
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

void APW_Lantern::HandleDrainFuel(float DeltaTime)
{
	if (_currentFuel <= 0.0f) return;
	ReduceFuel(_fuelDrainRate * DeltaTime);
}

void APW_Lantern::AddFuel(float amount)
{
	if (HasAuthority())
	{
		LocalModifyFuel(amount);
	}
	else
	{
		ServerModifyFuel(amount);
	}
}

void APW_Lantern::ReduceFuel(float amount)
{
	if (HasAuthority())
	{
		LocalModifyFuel(-amount);
	}
	else
	{
		ServerModifyFuel(-amount);
	}
}

void APW_Lantern::ServerModifyFuel_Implementation(float amount)
{
	if (!HasAuthority()) return;
	LocalModifyFuel(amount);
}

void APW_Lantern::LocalModifyFuel(float amount)
{
	_currentFuel = FMath::Clamp(_currentFuel + amount, 0.0f, _maxFuel);
}

void APW_Lantern::DIssolveEnemy(AActor* enemy)
{
	if (HasAuthority())
	{
		LocalDissolveEnemy(enemy);
	}
	else
	{
		ServerDissolveEnemy(enemy);
	}
}

void APW_Lantern::ServerDissolveEnemy_Implementation(AActor* enemy)
{
	if (!HasAuthority())
		return;
	LocalDissolveEnemy(enemy);
}

void APW_Lantern::LocalDissolveEnemy(AActor* enemy)
{
	UPW_MaterialEffectComponent* materialEffectComponent = enemy->FindComponentByClass<UPW_MaterialEffectComponent>();
	if (materialEffectComponent)
	{
		materialEffectComponent->ActivateEffect(EEffectDirection::ED_Forward);
	}
}

void APW_Lantern::ToggleLightVisibility(bool visible)
{
	_lightBeamMesh->SetVisibility(visible);
	_pointLight->SetVisibility(visible);
}

FVector APW_Lantern::GetLocationOfTarget()
{
	AActor* player = GetOwner ();
	if (!player) return FVector::ZeroVector;
	FVector targetLocation = FVector::ZeroVector;
	FVector directionToTarget = _target->GetActorLocation() - player->GetActorLocation();
	directionToTarget.Normalize();
	//targetLocation = _target->GetActorLocation() - directionToTarget * _target->GetActorRightVector() * 300;
	targetLocation = _target->GetActorLocation() - directionToTarget * 300;
	DRAW_SPHERE_SingleFrame(targetLocation);
	return targetLocation;
}

float APW_Lantern::GetNormalisedFuel()
{
	return _currentFuel / _maxFuel;
}
