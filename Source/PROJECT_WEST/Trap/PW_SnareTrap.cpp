// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_SnareTrap.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/PlayerController/PW_PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

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
}

void APW_SnareTrap::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);

	if (HasAuthority())
	{
		_triggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APW_SnareTrap::OnOverlapBegin);
	}
}

void APW_SnareTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	
	APW_Character* character = Cast<APW_Character>(OtherActor);
	if (character)
	{
		APW_PlayerController* playerController = Cast<APW_PlayerController>(character->GetController());
		if (playerController)
		{
			_isSnareActive = true;
			playerController->ClientTogglePlayerInput( false );


			character->GetCharacterMovement()->Velocity = FVector::ZeroVector;
			character->GetCharacterMovement()->SetMovementMode( EMovementMode::MOVE_None);
			character->GetCharacterMovement()->GravityScale =  0;
			character->GetMesh()->SetEnableGravity(false);
			character->AttachToComponent(_skeletalMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			character->SetActorLocation(_triggerVolume->GetComponentLocation());
			
			//playerController->ClientActivateTrapMode(this);
			_skeletalMesh->PlayAnimation(_catchAnimation, false);
			MoveTrapUp();
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

