// Fill out your copyright notice in the Description page of Project Settings.

#include "PW_LanternObject.h"

#include "PW_Character.h"
#include "PW_HealthComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "Gameplay/Components/PW_MaterialEffectComponent.h"
#include "Items/PW_Item.h"
#include "Net/UnrealNetwork.h"

APW_LanternObject::APW_LanternObject()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	UStaticMeshComponent* itemMesh = GetItemMesh();
	
	_lightBeamMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightBeamMesh"));
	_lightBeamMesh->SetupAttachment(itemMesh);
	_lightBeamMesh->SetIsReplicated(true);
	
	_pointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	_pointLight->SetupAttachment(itemMesh);
	_pointLight->SetIsReplicated(true);

	_bodyDetectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyDetectionBox"));
	_bodyDetectionBox->SetupAttachment(itemMesh);
	_bodyDetectionBox->SetIsReplicated(true);
	_bodyDetectionBox->SetCollisionProfileName("Trigger");

	_currentLightIntensity = _minLightIntensity = 1000.0f;
	_maxLightIntensity = 10000.0f;

	_currentBeamScale = _minBeamScale = .5f;
	_maxBeamScale = 2.0f;
	
	_maxFuel = 100.0f;
	_fuelPerCharge = 10.0f;
	_fuelDrainRate = 10.0f;
}

void APW_LanternObject::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);

	_pointLight->SetIntensity(_currentLightIntensity);
	_lightBeamMesh->SetRelativeScale3D(FVector(_currentBeamScale, _currentBeamScale, 1.0f));
	ToggleLightVisibility(false);
	_currentFuel = 0.0f;

	if (HasAuthority())
	{
		_bodyDetectionBox->OnComponentBeginOverlap.AddDynamic(this, &APW_LanternObject::OnBodyDetectionBoxBeginOverlap);
	}
}

void APW_LanternObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	HandleTargetDetection (DeltaTime);
	if (HasAuthority())
		HandleDrainFuel(DeltaTime);
}

void APW_LanternObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APW_LanternObject, _target);
	DOREPLIFETIME(APW_LanternObject, _currentFuel);
}

void APW_LanternObject::EnterHeldState()
{
	Super::EnterHeldState();
	ToggleLightVisibility(true);
}

void APW_LanternObject::EnterDroppedState()
{
	Super::EnterDroppedState();
	ToggleLightVisibility(false);
}

void APW_LanternObject::OnBodyDetectionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!_target || !GetIsActive() || GetItemState() == EItemObjectState::EDropped) return;
	
	if (OtherActor == nullptr || OtherActor == this) return;
	
	if (APW_Character* character = Cast<APW_Character>(OtherActor)) return;

	const UPW_HealthComponent* healthComponent = OtherActor->FindComponentByClass<UPW_HealthComponent>();
	
	if (healthComponent && !healthComponent->IsAlive())
	{
		AddFuel(_fuelPerCharge);
		
		UCapsuleComponent* capsuleComponent = OtherActor->FindComponentByClass<UCapsuleComponent>();
		if (capsuleComponent)
			capsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DissolveEnemy(OtherActor);
	}
}

void APW_LanternObject::HandleTargetDetection(float DeltaTime)
{
	if(!_target || GetIsActive()  || GetItemState() == EItemObjectState::EDropped) return;

	//_currentSearchDistance = FMath::Lerp (_minSearchDistance, _maxSearchDistance, _currentFuel / _maxFuel);
	
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

void APW_LanternObject::HandleLightIntensity(float normalisedAngle)
{
	_currentLightIntensity = FMath::Lerp (_minLightIntensity, _maxLightIntensity, normalisedAngle);
	_pointLight -> SetIntensity (1 - _currentLightIntensity);
}

void APW_LanternObject::HandleLightBeamScale(float normalisedAngle)
{
	_currentBeamScale = FMath::Lerp (_minBeamScale, _maxBeamScale, normalisedAngle);
	_lightBeamMesh -> SetRelativeScale3D (FVector (_currentBeamScale, _currentBeamScale, _lightBeamMesh -> GetRelativeScale3D ().Z));
}

void APW_LanternObject::HandleDrainFuel(float DeltaTime)
{
	if (_currentFuel <= 0.0f) return;
	LocalModifyFuel(_fuelDrainRate * DeltaTime);
}

void APW_LanternObject::AddFuel(float amount)
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

void APW_LanternObject::ReduceFuel(float amount)
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

void APW_LanternObject::ServerModifyFuel_Implementation(float amount)
{
	if (!HasAuthority()) return;
	LocalModifyFuel(amount);
}

void APW_LanternObject::LocalModifyFuel(float amount)
{
	_currentFuel = FMath::Clamp(_currentFuel + amount, 0.0f, _maxFuel);
}

void APW_LanternObject::DissolveEnemy(AActor* enemy)
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

void APW_LanternObject::ServerDissolveEnemy_Implementation(AActor* enemy)
{
	if (!HasAuthority())
		return;
	LocalDissolveEnemy(enemy);
}

void APW_LanternObject::LocalDissolveEnemy(AActor* enemy)
{
	UPW_MaterialEffectComponent* materialEffectComponent = enemy->FindComponentByClass<UPW_MaterialEffectComponent>();
	if (materialEffectComponent)
	{
		materialEffectComponent->ActivateEffect(EEffectDirection::ED_Forward);
	}
}

void APW_LanternObject::ToggleLightVisibility(bool visible)
{
	_lightBeamMesh->SetVisibility(visible);
	_pointLight->SetVisibility(visible);
}

float APW_LanternObject::GetNormalisedFuel()
{
	return _currentFuel / _maxFuel;
}





