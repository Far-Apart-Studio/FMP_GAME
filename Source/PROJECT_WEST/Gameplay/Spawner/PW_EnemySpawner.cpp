// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_EnemySpawner.h"

#include "Components/BoxComponent.h"
#include "PROJECT_WEST/DebugMacros.h"

APW_EnemySpawner::APW_EnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicates( true );

	//_spawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
	//_spawnArea->SetupAttachment(GetRootComponent());
	//_spawnArea->SetCollisionProfileName(FName("OverlapAllDynamic"));

	//_detectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectionBox"));
	//_detectionBox->SetupAttachment(GetRootComponent());
	//_detectionBox->SetCollisionProfileName(FName("OverlapAllDynamic"));
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
	DEBUG_STRING("APW_EnemySpawner::OnDetectionBoxBeginOverlap");
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
	AActor* actor = GetWorld()->SpawnActor<AActor> (actorClass, position, normal.Rotation());
	return actor;
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

