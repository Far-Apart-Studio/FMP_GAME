// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_EnemySpawner.h"

#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
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

	_spawnHeightOffset = 100.0f;
	_spawnChance = 0.5f;
	_spawnCooldown = 5.0f;
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

void APW_EnemySpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(APW_EnemySpawner, _spawnedActors);
	//DOREPLIFETIME(APW_EnemySpawner, _deadActors);
}

void APW_EnemySpawner::OnDetectionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APW_Character* character = Cast<APW_Character>(OtherActor);
	if (character)
	{
		_triggredActors.Add(OtherActor);
		
		if(CanSpawnEnemy())
			SpawnEnemies();
	}	
}

void APW_EnemySpawner::OnDetectionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APW_Character* character = Cast<APW_Character>(OtherActor);
	if (character)
	{
		_triggredActors.Remove(OtherActor);
	}
}

void APW_EnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

AActor* APW_EnemySpawner::SpawnActorInBox(TSubclassOf<AActor> actorClass)
{
	FVector position, normal;
	GetGroundPositionAndNormal(GetRandomPositionInSpawnArea(),position, normal);
	
	AActor* actorToSpawn  = GetWorld()->SpawnActor<AActor> (actorClass, position, FRotator::ZeroRotator);
	if (actorToSpawn)
	{
		TryFadeActorMaterial(actorToSpawn);
		TryAssignDeathEvent(actorToSpawn);
		_spawnedActors.Add(actorToSpawn);
	}
	
	return actorToSpawn;
}

void APW_EnemySpawner::SpawnEnemies()
{
	TArray<FEnemySpawnInfo> spawnInfo = _actorsToSpawn.GetRandomActorClass();
	if (spawnInfo.Num() > 0)
	{
		for (const FEnemySpawnInfo& info : spawnInfo)
		{
			for (int i = 0; i < info._amount; i++)
			{
				SpawnActorInBox(info._actorClass);
			}
		}
	}
}

bool APW_EnemySpawner::CanSpawnEnemy()
{
	return _spawnChance > FMath::FRand();
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

void APW_EnemySpawner::OnActorDeath(AActor* OwnerActor,AActor* DamageCauser, AController* DamageCauserController)
{
	_spawnedActors.Remove(OwnerActor);
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

	FCollisionQueryParams params;
	params.AddIgnoredActor(GetOwner());
	if (GetWorld()->LineTraceSingleByChannel(hit, origin, end, ECC_Visibility, params))
	{
		outPosition = hit.ImpactPoint + FVector::UpVector * _spawnHeightOffset;
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

