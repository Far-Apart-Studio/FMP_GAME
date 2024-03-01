// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_AutoEnemySpawner.h"
#include "PW_BoxSpawningComponent.h"
#include "PW_EnemySpawner.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/PW_HealthComponent.h"
#include "PROJECT_WEST/Gameplay/Components/PW_DistanceUnloaderComponent.h"
#include "PROJECT_WEST/Gameplay/Components/PW_MaterialEffectComponent.h"

APW_AutoEnemySpawner::APW_AutoEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicates( true );

	_root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(_root);
	
	_spawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
	_spawnArea->SetupAttachment(_root);

	_boxSpawningComponent = CreateDefaultSubobject<UPW_BoxSpawningComponent>(TEXT("BoxSpawningComponent"));
	_boxSpawningComponent->SetBoxComponent(_spawnArea);

	_spawnRate = 5.0f;
	_maxSpawnedEnemies = 5;
	_minDistanceToPlayer = 1000;
	_whileLoopBreakerMaxCount = 50;
	_distanceFromPlayer = 5000;
	_zpositionOffset = 1000;
}

void APW_AutoEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	_spawnTimer = _spawnRate;
}

void APW_AutoEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasAuthority() && _character && _healthComponent && _healthComponent->IsAlive())
	{
		HandleSpawnTimer(DeltaTime);
		HandlePositioning();
	}
}

void APW_AutoEnemySpawner::SetCharacter(ACharacter* character)
{
	_character = character;
	_healthComponent = character->FindComponentByClass<UPW_HealthComponent>();
}

void APW_AutoEnemySpawner::HandleSpawnTimer(float DeltaTime)
{
	if (_spawnTimer > 0)
	{
		_spawnTimer -= DeltaTime;
	}
	else
	{
		if (_spawnedActors.Num() < _maxSpawnedEnemies)
		{
			SpawnEnemies();
			_spawnTimer = _spawnRate;
		}
		else
		{
			//DEBUG_STRING("Max enemies spawned");
		}
	}
}

void APW_AutoEnemySpawner::HandlePositioning()
{
	if (_character)
	{
		FVector playerMovingDirection = _character->GetVelocity();
		playerMovingDirection.Z = 0;
		playerMovingDirection.Normalize();
		FVector newLocation = _character->GetActorLocation() + playerMovingDirection * _distanceFromPlayer;
		SetActorLocation(newLocation + FVector(0, 0, _zpositionOffset));
		FRotator newRotation = playerMovingDirection.Rotation();
		SetActorRotation(newRotation);
	}
}

void APW_AutoEnemySpawner::SpawnEnemies()
{
	TArray<FSpawnInfo> spawnInfo = _weightedSpawn.GetRandomActorClass();
	FVector spawnPosition = FVector::ZeroVector;
	if(!GetValidSpawnPosition(spawnPosition))
		return;
	
	if (spawnInfo.Num() > 0)
	{
		for (const FSpawnInfo& info : spawnInfo)
		{
			for (int i = 0; i < info._amount; i++)
			{
				AActor* actorToSpawn = GetWorld()->SpawnActor<AActor> (info._actorClass, spawnPosition, FRotator::ZeroRotator);
				if (actorToSpawn)
				{
					TryAssignDeathEvent(actorToSpawn);
					TryAssignUnloaderEvent(actorToSpawn);
					_spawnedActors.Add(actorToSpawn);
				}
			}
		}
	}

	//DEBUG_STRING( "Spawn position: " + FString::SanitizeFloat(_spawnedActors.Num()));
}

bool APW_AutoEnemySpawner::GetValidSpawnPosition(FVector& outPosition)
{
	bool isValid = false;
	FVector position = FVector::ZeroVector;
	float whileLoopBreaker = 0;
	
	do
	{
		isValid = _boxSpawningComponent->GetGroundPositionAndNormal(_boxSpawningComponent->GetRandomPositionInBox(), position);
		whileLoopBreaker++;
	}
	while ((!isValid || AnyPlayerInRange(position)) && whileLoopBreaker < 100);
	
	outPosition = position;

	if (whileLoopBreaker >= 100)
	{
		return false;
	}
	
	return true;
}

bool APW_AutoEnemySpawner::AnyPlayerInRange(FVector location)
{
	bool result = false;
	TArray< AActor* > players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APW_Character::StaticClass(), players);
	
	for (AActor* player : players)
	{
		float distance = FVector::Dist(location, player->GetActorLocation());
		if (distance < _minDistanceToPlayer)
		{
			result = true;
			break;
		}
	}
	
	return result;
}

void APW_AutoEnemySpawner::TryAssignDeathEvent(AActor* actor)
{
	UPW_HealthComponent* healthComponent = actor->FindComponentByClass<UPW_HealthComponent>();
	if (healthComponent)
	{
		healthComponent->OnDeath.AddDynamic(this, &APW_AutoEnemySpawner::OnActorDeath);
	}
}

void APW_AutoEnemySpawner::TryFadeActorMaterial(AActor* actor)
{
	UPW_MaterialEffectComponent* materialEffectComponent = actor->FindComponentByClass<UPW_MaterialEffectComponent>();
	if (materialEffectComponent)
	{
		materialEffectComponent->ActivateEffect(EEffectDirection::ED_Backward);
	}
}

void APW_AutoEnemySpawner::TryAssignUnloaderEvent(AActor* actor)
{
	UPW_DistanceUnloaderComponent* unloaderComponent = actor->FindComponentByClass<UPW_DistanceUnloaderComponent>();
	if (unloaderComponent)
	{
		unloaderComponent->_onUnloaded.AddDynamic(this, &APW_AutoEnemySpawner::OnActorUnloaded);
	}
}

void APW_AutoEnemySpawner::OnActorDeath(AActor* OwnerActor,AActor* DamageCauser, AController* DamageCauserController)
{
	_spawnedActors.Remove(OwnerActor);
}

void APW_AutoEnemySpawner::OnActorUnloaded(AActor* UnloadedActor)
{
	if (_spawnedActors.Contains(UnloadedActor))
		_spawnedActors.Remove(UnloadedActor);
}

