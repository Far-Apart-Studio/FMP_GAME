// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_TransitionPortal.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/GameModes/PW_LobbyGameMode.h"
#include "Net/UnrealNetwork.h"
#include "PROJECT_WEST/DebugMacros.h"

// Sets default values
APW_TransitionPortal::APW_TransitionPortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	_root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent( _root );

	_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	_mesh->SetupAttachment( _root);

	_extractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ExtractionBox"));
	_extractionBox->SetupAttachment( _root);
	_extractionBox->SetIsReplicated(true);
	_extractionBox->SetCollisionProfileName(FName("OverlapAllDynamic"));
}

void APW_TransitionPortal::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		_lobbyGameMode = Cast<APW_LobbyGameMode>(GetWorld()->GetAuthGameMode());
	}

	OnActivationChangedDelegate.Broadcast(_isActivated);
}

void APW_TransitionPortal::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APW_TransitionPortal, _isActivated);
}

void APW_TransitionPortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckForOverlap();
}

void APW_TransitionPortal::ToggleActivation(bool value)
{
	_isActivated = value;
	OnActivationChangedDelegate.Broadcast(_isActivated);
}

void APW_TransitionPortal::CheckForOverlap()
{
	if(!HasAuthority() || _winCondition || !_isActivated) return;
	
	TArray<AActor*> overlappingActors;
	_extractionBox->GetOverlappingActors(overlappingActors, APW_Character::StaticClass());
	
	_winCondition = overlappingActors.Num() >= _lobbyGameMode->GetNumPlayerInSession();
	if (_winCondition)
	{
		OnSuccess.Broadcast();
		_lobbyGameMode->ServerTravel(_mapPath);
	}
}

void APW_TransitionPortal::OnRep_ActivationChanged()
{
	OnActivationChangedDelegate.Broadcast(_isActivated);
}

