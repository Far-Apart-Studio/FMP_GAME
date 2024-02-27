// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_SnareTrap.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/ArrowComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/PlayerController/PW_PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "PROJECT_WEST/Gameplay/Components/PW_ActorMoverComponent.h"
#include "PROJECT_WEST/PW_HealthComponent.h"

APW_SnareTrap::APW_SnareTrap()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent( _mesh );
	_mesh->SetIsReplicated(true);
	
	_skeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	_skeletalMesh->SetupAttachment(RootComponent);
	
	_triggerVolume = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerVolume"));
	_triggerVolume->SetupAttachment(RootComponent);
	_triggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_triggerVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	_triggerVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	_point1 = CreateDefaultSubobject<UArrowComponent>(TEXT("Point1"));
	_point1->SetupAttachment(RootComponent);

	_point2 = CreateDefaultSubobject<UArrowComponent>(TEXT("Point2"));
	_point2->SetupAttachment(RootComponent);

	_actorMoverComponent = CreateDefaultSubobject<UPW_ActorMoverComponent>(TEXT("ActorMoverComponent"));
	
	_damageRate = 10.0f;
}

void APW_SnareTrap::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);

	if (HasAuthority())
	{
		_triggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APW_SnareTrap::OnOverlapBegin);
	}

	FVector startPoint = GetActorLocation() + _point1->GetRelativeLocation();
	FVector endPoint =  GetActorLocation() + _point2->GetRelativeLocation();
	_actorMoverComponent->SetPoints(startPoint, endPoint);
}

void APW_SnareTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasAuthority())
	{
		DrainHealthOfCaughtCharacter(DeltaTime);
	}
}

void APW_SnareTrap::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APW_SnareTrap, _isSnareActive);
}

void APW_SnareTrap::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (_isSnareActive)
	{ 
		return;
	}
	
	_caughtCharacter = Cast<APW_Character>(OtherActor);
	if (_caughtCharacter)
	{
		APW_PlayerController* playerController = Cast<APW_PlayerController>(_caughtCharacter->GetController());
		if (playerController)
		{
			_isSnareActive = true;
			playerController->ClientTogglePlayerInput( false );


			_caughtCharacter->GetCharacterMovement()->Velocity = FVector::ZeroVector;
			_caughtCharacter->GetCharacterMovement()->SetMovementMode( EMovementMode::MOVE_None);
			_caughtCharacter->GetCharacterMovement()->GravityScale =  0;
			_caughtCharacter->GetMesh()->SetEnableGravity(false);
			_caughtCharacter->AttachToComponent(_skeletalMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			_caughtCharacter->SetActorLocation(_triggerVolume->GetComponentLocation());
			
			//playerController->ClientActivateTrapMode(this);
			_skeletalMesh->PlayAnimation(_catchAnimation, false);
			_actorMoverComponent->SetCanMove(true);
		}
	}
}

void APW_SnareTrap::OnRep_OnStatsChanged()
{
	_skeletalMesh->PlayAnimation(_catchAnimation, false);	
}

void APW_SnareTrap::MoveTrapUp()
{
	float newZ = GetActorLocation().Z + 500.0f;
	SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, newZ));
}

void APW_SnareTrap::DrainHealthOfCaughtCharacter(float DeltaTime)
{
	if(!_caughtCharacter) return;
	
	UPW_HealthComponent* healthComponent = _caughtCharacter->FindComponentByClass<UPW_HealthComponent>();
	if(healthComponent && healthComponent->IsAlive())
	{
		_caughtCharacter->TakeDamage(_damageRate * DeltaTime, FDamageEvent(), nullptr, this);
		if (!healthComponent->IsAlive())
		{
			_actorMoverComponent->SetMoveDirection(EMoveDirection::EMD_Backward);
			_caughtCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			_caughtCharacter->GetMesh()->SetEnableGravity(true);
			_caughtCharacter->GetCharacterMovement()->GravityScale = 1;
			_caughtCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		}
	}
}

