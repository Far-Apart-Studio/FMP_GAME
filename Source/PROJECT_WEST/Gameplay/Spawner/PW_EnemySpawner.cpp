// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_EnemySpawner.h"

#include "PW_BoxSpawningComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/PW_HealthComponent.h"
#include "PROJECT_WEST/Gameplay/Components/PW_DistanceUnloaderComponent.h"
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

	_boxSpawningComponent = CreateDefaultSubobject<UPW_BoxSpawningComponent>(TEXT("BoxSpawningComponent"));
	_boxSpawningComponent->SetBoxComponent(_spawnArea);
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

void APW_EnemySpawner::OnDetectionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APW_Character* character = Cast<APW_Character>(OtherActor);
	if (character)
	{
		if(CanSpawnEnemy())
			SpawnEnemies();
		
		_triggredActors.Add(OtherActor);
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

AActor* APW_EnemySpawner::SpawnActor(TSubclassOf<AActor> actorClass)
{
	FVector spawnPosition = FVector::ZeroVector;
	_boxSpawningComponent->GetGroundPositionAndNormal(_boxSpawningComponent->GetRandomPositionInBox(), spawnPosition);
	AActor* actorToSpawn = GetWorld()->SpawnActor<AActor> (actorClass, spawnPosition, FRotator::ZeroRotator);
	if (actorToSpawn)
	{
		TryAssignDeathEvent(actorToSpawn);
		TryAssignUnloaderEvent(actorToSpawn);
		_spawnedActors.Add(actorToSpawn);
	}
	return actorToSpawn;
}

void APW_EnemySpawner::SpawnEnemies()
{
	for (int i = 0; i < GetNumberOfCharactersInLevel(); i++)
	{
		TArray<FSpawnInfo> spawnInfo = _weightedSpawn.GetRandomActorClass();
		if (spawnInfo.Num() > 0)
		{
			for (const FSpawnInfo& info : spawnInfo)
			{
				for (int j = 0; j < info._amount; j++)
				{
					SpawnActor(info._actorClass);
				}
			}
		}
	}
}

bool APW_EnemySpawner::CanSpawnEnemy()
{
	return _triggredActors.Num() == 0 && _spawnedActors.Num() == 0;
}

int APW_EnemySpawner::GetNumberOfCharactersInLevel()
{
	int result = 0;
	TArray< AActor* > players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APW_Character::StaticClass(), players);
	for (AActor* player : players)
	{
		UPW_HealthComponent* healthComponent = player->FindComponentByClass<UPW_HealthComponent>();
		if (!healthComponent || !healthComponent->IsAlive())
			continue;
		result++;
	}
	return result;
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

void APW_EnemySpawner::TryAssignUnloaderEvent(AActor* actor)
{
	UPW_DistanceUnloaderComponent* unloaderComponent = actor->FindComponentByClass<UPW_DistanceUnloaderComponent>();
	if (unloaderComponent)
	{
		unloaderComponent->_onUnloaded.AddDynamic(this, &APW_EnemySpawner::OnActorUnloaded);
	}
}

void APW_EnemySpawner::OnActorDeath(AActor* OwnerActor,AActor* DamageCauser, AController* DamageCauserController)
{
	_spawnedActors.Remove(OwnerActor);
}

void APW_EnemySpawner::OnActorUnloaded(AActor* UnloadedActor)
{
	if (_spawnedActors.Contains(UnloadedActor))
	_spawnedActors.Remove(UnloadedActor);
}
