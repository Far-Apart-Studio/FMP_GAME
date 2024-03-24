// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_ExtractionPoint.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerState.h"
#include  "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/PW_InventoryHandler.h"
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
}

// Called when the game starts or when spawned
void APW_ExtractionPoint::BeginPlay()
{
	Super::BeginPlay();
	_canInteract = true;

	if (HasAuthority())
	{
		_extractionBox->OnComponentBeginOverlap.AddDynamic(this, &APW_ExtractionPoint::OnEnterExtractionBox);
	}
}

// Called every frame
void APW_ExtractionPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//CheckForWin();
}

void APW_ExtractionPoint::StartFocus_Implementation(AActor* owner)
{
	DEBUG_STRING( "ExtractionPoint Focused" );
}

void APW_ExtractionPoint::EndFocus_Implementation(AActor* owner)
{
	DEBUG_STRING ( "ExtractionPoint Unfocused" );
}

void APW_ExtractionPoint::StartInteract_Implementation(AActor* owner)
{

}

bool APW_ExtractionPoint::HasServerInteraction_Implementation()
{
	return true;
}

void APW_ExtractionPoint::ServerStartInteract_Implementation(AActor* owner)
{
	CheckForWin();
}

void APW_ExtractionPoint::OnEnterExtractionBox(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APW_Character* character = Cast<APW_Character>(OtherActor))
	{
		if(!_triggered)
		{
			_triggered = true;
			OnPlayerTrigger.Broadcast(character);
		}
	}
}

void APW_ExtractionPoint::CheckForWin()
{
	if(!_canInteract) return;
	TArray<AActor*> overlappingActors;
	_extractionBox->GetOverlappingActors(overlappingActors, APW_Character::StaticClass());
	
	if(overlappingActors.Num() > 0)
	{
		_canInteract = false;
		bool _winCondition = false;
		TArray<FString> escapedPlayers;
		
		for (AActor* actor : overlappingActors)
		{
			APW_Character* character = Cast<APW_Character>(actor);
			if (character)
			{
				escapedPlayers.Add(character->GetPlayerState()->GetPlayerName());
				
				UPW_InventoryHandler* inventoryHandler = Cast <UPW_InventoryHandler>(character->GetComponentByClass(UPW_InventoryHandler::StaticClass()));
				if (inventoryHandler && inventoryHandler->HasItemType<APW_BountyHead>())
				{
					_winCondition = true;
					break;
				}
				
			}
		}
	
		OnWinConditionMet.Broadcast( _winCondition,escapedPlayers );
	}
}

