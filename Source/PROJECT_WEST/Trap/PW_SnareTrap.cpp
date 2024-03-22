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
#include "PROJECT_WEST/Gameplay/Components/PW_MaterialEffectComponent.h"

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

	_healthComponent = CreateDefaultSubobject<UPW_HealthComponent>(TEXT("HealthComponent"));

	_materialEffectComponent = CreateDefaultSubobject<UPW_MaterialEffectComponent>(TEXT("MaterialEffectComponent"));
	
	_damageRate = 10.0f;

	_damageDuration = 5.0f;
}

void APW_SnareTrap::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);

	if (HasAuthority())
	{
		_triggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APW_SnareTrap::OnOverlapBegin);
		_healthComponent->OnHealthChangedGlobal.AddDynamic(this, &APW_SnareTrap::OnHealthChanged);
		_healthComponent->OnDeathGlobal.AddDynamic(this, &APW_SnareTrap::OnDeath);
		_materialEffectComponent->_onHighlightComplete.AddDynamic(this, &APW_SnareTrap::OnHighlightComplete);
	}

	FVector startPoint = GetActorLocation() + _point1->GetRelativeLocation();
	FVector endPoint =  GetActorLocation() + _point2->GetRelativeLocation();
	_actorMoverComponent->SetPoints(startPoint, endPoint);

	ToggleCollisionQuery(false);
}

void APW_SnareTrap::OnHealthChanged()
{
	//DEBUG_STRING("Snare trap health changed");
}

void APW_SnareTrap::OnDeath(AActor* OwnerActor, AActor* DamageCauser, AController* DamageCauserController)
{
	if (_isDeactivated)
	{
		return;
	}

	_isDeactivated = true;

	_isSnareTriggered = false;
	
	_actorMoverComponent->SetMoveDirection(EMoveDirection::EMD_Backward);
	
	if(_caughtCharacter)
	{
		APW_PlayerController* playerController = Cast<APW_PlayerController>(_caughtCharacter->GetController());
		if (playerController && _caughtCharacterHealthComponent && _caughtCharacterHealthComponent->IsAlive())
		{
			playerController->ClientTogglePlayerInput( true );
		}
	
		_caughtCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		_caughtCharacter->GetMesh()->SetEnableGravity(true);
		_caughtCharacter->GetCharacterMovement()->GravityScale = 1;
		_caughtCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

		_skeletalMesh->PlayAnimation(_releaseAnimation, false);
	}

	_materialEffectComponent->ActivateEffect(EEffectDirection::ED_Forward);

	_caughtCharacter = nullptr;

	_OnSnareDeactivated.Broadcast();
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
	DOREPLIFETIME(APW_SnareTrap, _isSnareTriggered);
}

void APW_SnareTrap::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (_isDeactivated || !_healthComponent->IsAlive())
	{ 
		return;
	}
	
	CaughtCharacter(Cast<APW_Character>(OtherActor));
}

void APW_SnareTrap::OnRep_OnStatsChanged()
{
	_skeletalMesh->PlayAnimation( _isSnareTriggered ? _catchAnimation : _releaseAnimation, false);
}

void APW_SnareTrap::OnHighlightComplete(EEffectDirection Direction)
{
	if (Direction == EEffectDirection::ED_Forward)
	{
		Destroy();
	}
}

void APW_SnareTrap::CaughtCharacter(APW_Character* Character)
{
	if (Character)
	{
		_caughtCharacter = Character;

		_caughtCharacterHealthComponent = _caughtCharacter->FindComponentByClass<UPW_HealthComponent>();
		
		APW_PlayerController* playerController = Cast<APW_PlayerController>(_caughtCharacter->GetController());
		if (playerController)
		{
			_isSnareTriggered = true;
			playerController->ClientTogglePlayerInput( false );
			
			_caughtCharacter->GetCharacterMovement()->Velocity = FVector::ZeroVector;
			_caughtCharacter->GetCharacterMovement()->SetMovementMode( EMovementMode::MOVE_None);
			_caughtCharacter->GetCharacterMovement()->GravityScale =  0;
			_caughtCharacter->GetMesh()->SetEnableGravity(false);
			_caughtCharacter->AttachToComponent(_skeletalMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			_caughtCharacter->SetActorLocation(_triggerVolume->GetComponentLocation() + FVector(0,0,100));
			_caughtCharacter->StopCrouching();
			
			_skeletalMesh->PlayAnimation(_catchAnimation, false);
			_actorMoverComponent->SetCanMove(true);

			ToggleCollisionQuery(true);
		}
		_OnSnareActivated.Broadcast();
	}
}

void APW_SnareTrap::DrainHealthOfCaughtCharacter(float DeltaTime)
{
	if(!_caughtCharacter || !_isSnareTriggered)
		return;
	
	if (!_caughtCharacterHealthComponent)
		return;
	
	if(_caughtCharacterHealthComponent->IsAlive())
	{
		_caughtCharacter->TakeDamage(_damageRate * DeltaTime, FDamageEvent(), nullptr, this);
	}
	
	_damageDuration -= DeltaTime;
	
	if(!_caughtCharacterHealthComponent->IsAlive() || _damageDuration <= 0)
	{
		OnDeath(nullptr, nullptr, nullptr);
	}
}

void APW_SnareTrap::ToggleCollisionQuery(bool bEnable)
{
	_mesh->SetCollisionEnabled(bEnable ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
}

