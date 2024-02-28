// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_EnemySpawner.h"

#include "Components/BoxComponent.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/PW_HealthComponent.h"
#include "PROJECT_WEST/Gameplay/Components/PW_MaterialEffectComponent.h"

APW_EnemySpawner::APW_EnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicates( true );

	_root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(_root);

	_spawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
	_spawnArea->SetupAttachment(_root);
	_spawnArea->SetCollisionProfileName(FName("OverlapAllDynamic"));

	_detectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectionBox"));
	_detectionBox->SetupAttachment(_root);
	_detectionBox->SetCollisionProfileName(FName("OverlapAllDynamic"));
}

void APW_EnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
	if (_detectionBox && HasAuthority())
	{
		_detectionBox->OnComponentBeginOverlap.AddDynamic(this, &APW_EnemySpawner::OnDetectionBoxBeginOverlap);
		_detectionBox->OnComponentEndOverlap.AddDynamic(this, &APW_EnemySpawner::OnDetectionBoxEndOverlap);
	}
}

void APW_EnemySpawner::OnDetectionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APW_Character* character = Cast<APW_Character>(OtherActor);
	if (character)
	{
		SpawnActorInBox(_actorsToSpawn.GetRandomActorClass());
	}	
}

void APW_EnemySpawner::OnDetectionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void APW_EnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

AActor* APW_EnemySpawner::SpawnActorInBox(TSubclassOf<AActor> actorClass)
{
	FVector position, normal;
	GetGroundPositionAndNormal(GetRandomPositionInSpawnArea(),position, normal);
	AActor* actorToSpawn = TryGetDeadActor(actorClass);
	if (!actorToSpawn)
	{
		actorToSpawn = GetWorld()->SpawnActor<AActor> (actorClass, position, normal.Rotation());
		TryFadeActorMaterial(actorToSpawn);
		TryAssignDeathEvent(actorToSpawn);
	}
	if (actorToSpawn)
	{
		_spawnedActors.Add(actorToSpawn);
	}
	return actorToSpawn;
}

void APW_EnemySpawner::TryAssignDeathEvent(AActor* actor)
{
	UPW_HealthComponent* healthComponent = actor->FindComponentByClass<UPW_HealthComponent>();
	if (healthComponent)
	{
		healthComponent->OnDeath.AddDynamic(this, &APW_EnemySpawner::OnActorDeath);
	}
}

void APW_EnemySpawner::TryFadeActorMaterial(AActor* actor)
{
	UPW_MaterialEffectComponent* materialEffectComponent = actor->FindComponentByClass<UPW_MaterialEffectComponent>();
	if (materialEffectComponent)
	{
		materialEffectComponent->ActivateEffect(EEffectDirection::ED_Backward);
	}
}

void APW_EnemySpawner::OnActorDeath(AActor* DamageCauser, AController* DamageCauserController)
{
	_spawnedActors.Remove(DamageCauser);
	_deadActors.Add(DamageCauser);
}

AActor* APW_EnemySpawner::TryGetDeadActor(TSubclassOf<AActor> actorClass)
{
	AActor* result = nullptr;
	if (_deadActors.Num() > 0 && actorClass)
	{
		for (AActor* actor : _deadActors)
		{
			if (actor->IsA(actorClass))
			{
				result = actor;
				break;
			}
		}
	}
	_deadActors.Remove(result);
	return result;
}

FVector APW_EnemySpawner::GetRandomPositionInSpawnArea()
{
	FVector result = FVector::ZeroVector;
	if (_spawnArea)
	{
		FVector origin = _spawnArea->GetComponentLocation();
		FVector extent = _spawnArea->GetScaledBoxExtent();
		result = FMath::RandPointInBox(FBox(origin - extent, origin + extent));
	}
	return result;
}

bool APW_EnemySpawner::IsPositionValid(FVector position)
{
	FHitResult hit;
	FCollisionQueryParams params;
	params.AddIgnoredActor(GetOwner());
	return !GetWorld()->SweepSingleByChannel(hit, position, position, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(50.0f), params);
}

void APW_EnemySpawner::GetGroundPositionAndNormal(FVector origin, FVector& outPosition, FVector& outNormal)
{
	FHitResult hit;
	FVector end = origin - FVector::UpVector * 1000.0f;
	const float outPositionZOffset = 100.f;
	
	FCollisionQueryParams params;
	params.AddIgnoredActor(GetOwner());
	if (GetWorld()->LineTraceSingleByChannel(hit, origin, end, ECC_Visibility, params))
	{
		outPosition = hit.ImpactPoint + FVector::UpVector * outPositionZOffset;
		outNormal = hit.ImpactNormal;
		
		DRAW_LINE(origin, outPosition);
		DRAW_SPHERE(hit.ImpactPoint);
	}
	else
	{
		outPosition = origin;
		outNormal = FVector::UpVector;
	}
}

