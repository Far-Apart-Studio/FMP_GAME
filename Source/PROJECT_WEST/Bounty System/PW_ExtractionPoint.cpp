// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_ExtractionPoint.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include  "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/PW_ItemHandlerComponent.h"
#include "PROJECT_WEST/Items/PW_BountyHead.h"


// Sets default values
APW_ExtractionPoint::APW_ExtractionPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicates( true );

	_root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent( _root );
	
	_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	_mesh->SetupAttachment( _root);

	_becon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Becon"));
	_becon->SetupAttachment( _root);
	
	_extractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ExtractionBox"));
	_extractionBox->SetupAttachment( _root);
	_extractionBox->SetIsReplicated(true);
	_extractionBox->SetCollisionProfileName(FName("OverlapAllDynamic"));
	
	_winCondition = false;
}

// Called when the game starts or when spawned
void APW_ExtractionPoint::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APW_ExtractionPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckForWin();
}

void APW_ExtractionPoint::CheckForWin()
{
	if(!HasAuthority() || _winCondition) return;
	TArray<AActor*> overlappingActors;
	_extractionBox->GetOverlappingActors(overlappingActors, APW_Character::StaticClass());

	for (AActor* actor : overlappingActors)
	{
		APW_Character* character = Cast<APW_Character>(actor);
		if (character)
		{
			UPW_ItemHandlerComponent* itemHandler = Cast <UPW_ItemHandlerComponent>(character->GetComponentByClass(UPW_ItemHandlerComponent::StaticClass()));
			if (itemHandler && itemHandler->HasItemType<APW_BountyHead>())
			{
				_winCondition = true;
				break;
			}
		}
	}
	
	if (_winCondition)
	{
		OnWinConditionMet.Broadcast( _winCondition );
		//MulticastRPCWin();
	}
}

void APW_ExtractionPoint::MulticastRPCWin_Implementation()
{
	DEBUG_STRING( "APW_ExtractionPoint::MulticastRPCWin_Implementation" );
	OnWinConditionMet.Broadcast( _winCondition );
}

