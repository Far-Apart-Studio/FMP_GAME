// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_PoiArea.h"
#include "PROJECT_WEST/Gameplay/Spawner/PW_BoxSpawningComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/PW_HealthComponent.h"
#include "PROJECT_WEST/Gameplay/Components/PW_DistanceUnloaderComponent.h"
#include "PROJECT_WEST/Gameplay/Components/PW_MaterialEffectComponent.h"
#include "PROJECT_WEST/PlayerController/PW_PlayerController.h"

APW_PoiArea::APW_PoiArea()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicates( true );

	_root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(_root);

	_spawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
	_spawnArea->SetupAttachment(_root);
	_spawnArea->SetCollisionProfileName(FName("OverlapAllDynamic"));

	_detectionTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionTrigger"));
	_detectionTrigger->SetupAttachment(_root);
	_detectionTrigger->SetCollisionProfileName(FName("OverlapAllDynamic"));

	_boxSpawningComponent = CreateDefaultSubobject<UPW_BoxSpawningComponent>(TEXT("BoxSpawningComponent"));
	_boxSpawningComponent->SetBoxComponent(_spawnArea);
}

void APW_PoiArea::BeginPlay()
{
	Super::BeginPlay();
	
	if (_detectionTrigger && HasAuthority())
	{
		_detectionTrigger->OnComponentBeginOverlap.AddDynamic(this, &APW_PoiArea::OnDetectionBoxBeginOverlap);
		_detectionTrigger->OnComponentEndOverlap.AddDynamic(this, &APW_PoiArea::OnDetectionBoxEndOverlap);
	}
}

void APW_PoiArea::OnDetectionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APW_Character* character = Cast<APW_Character>(OtherActor);
	if (character)
	{
		if (!_isTriggered)
		{
			_isTriggered = true;
			OnPoiTriggered.Broadcast(this);
		}

		APW_PlayerController* playerController = Cast<APW_PlayerController>(character->GetController());
		if (playerController)
		{
			FNotificationEntry notification;
			notification._notificationType = ENotificationType::ELocation;
			notification._playerNameText ="";
			notification._notificationText = "You have entered " +  _poiID;
			playerController->TriggerNotification(notification);
		}
		
		if(CanSpawnEnemy())
			SpawnEnemies();
		
		_triggeredActors.Add(OtherActor);
	}	
}

void APW_PoiArea::OnDetectionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APW_Character* character = Cast<APW_Character>(OtherActor);
	if (character)
	{
		_triggeredActors.Remove(OtherActor);
	}
}

void APW_PoiArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

AActor* APW_PoiArea::SpawnPOIEnemy(TSubclassOf<AActor> actorClass)
{
	FVector spawnPosition = FVector::ZeroVector;
	_boxSpawningComponent->GetGroundPositionAndNormal(_boxSpawningComponent->GetRandomPositionInBox(), spawnPosition);
	AActor* actorToSpawn = GetWorld()->SpawnActor<AActor> (actorClass, spawnPosition, FRotator::ZeroRotator);
	if (actorToSpawn)
	{
		TryAssignDeathEvent(actorToSpawn);
		TryAssignUnloaderEvent(actorToSpawn);
		_spawnedEnemies.Add(actorToSpawn);
	}
	return actorToSpawn;
}

AActor* APW_PoiArea::SpawnActor(TSubclassOf<AActor> actorClass)
{
	FVector spawnPosition = FVector::ZeroVector;
	_boxSpawningComponent->GetGroundPositionAndNormal(_boxSpawningComponent->GetRandomPositionInBox(), spawnPosition);
	return GetWorld()->SpawnActor<AActor> (actorClass, spawnPosition, FRotator::ZeroRotator);
}

void APW_PoiArea::SpawnEnemies()
{
	for (int i = 0; i < GetNumberOfCharactersInLevel(); i++)
	{
		TArray<FSpawnInfo> spawnInfo = _enemyWeightedSpawn.GetRandomActorClass();
		if (spawnInfo.Num() > 0)
		{
			for (const FSpawnInfo& info : spawnInfo)
			{
				for (int j = 0; j < info._amount; j++)
				{
					SpawnPOIEnemy(info._actorClass);
				}
			}
		}
	}
}

bool APW_PoiArea::CanSpawnEnemy()
{
	return _triggeredActors.Num() == 0 && _spawnedEnemies.Num() == 0;
}

int APW_PoiArea::GetNumberOfCharactersInLevel()
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

void APW_PoiArea::TryAssignDeathEvent(AActor* actor)
{
	UPW_HealthComponent* healthComponent = actor->FindComponentByClass<UPW_HealthComponent>();
	if (healthComponent)
	{
		healthComponent->OnDeathGlobal.AddDynamic(this, &APW_PoiArea::OnEnemyDeath);
	}
}

void APW_PoiArea::TryFadeActorMaterial(AActor* actor)
{
	UPW_MaterialEffectComponent* materialEffectComponent = actor->FindComponentByClass<UPW_MaterialEffectComponent>();
	if (materialEffectComponent)
	{
		materialEffectComponent->ActivateEffect(EEffectDirection::ED_Backward);
	}
}

void APW_PoiArea::TryAssignUnloaderEvent(AActor* actor)
{
	UPW_DistanceUnloaderComponent* unloaderComponent = actor->FindComponentByClass<UPW_DistanceUnloaderComponent>();
	if (unloaderComponent)
	{
		unloaderComponent->SetCanUnload(true);
		unloaderComponent->_onUnloaded.AddDynamic(this, &APW_PoiArea::OnActorUnloaded);
	}
}

void APW_PoiArea::OnEnemyDeath(AActor* OwnerActor,AActor* DamageCauser, AController* DamageCauserController)
{
	_spawnedEnemies.Remove(OwnerActor);
}

void APW_PoiArea::OnActorUnloaded(AActor* UnloadedActor)
{
	if (_spawnedEnemies.Contains(UnloadedActor))
	_spawnedEnemies.Remove(UnloadedActor);
}
