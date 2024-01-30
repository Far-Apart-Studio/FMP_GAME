// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_BountyBoard.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
APW_BountyBoard::APW_BountyBoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicates(true);

	_root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(_root);
	
	_bountyBoardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BountyBoardMesh"));
	_bountyBoardMesh->SetupAttachment(_root);

	_bountyBoardWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("BountyBoardWidget"));
	_bountyBoardWidget->SetupAttachment(_root);
	_bountyBoardWidget->SetIsReplicated(true);
	_bountyBoardWidget->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void APW_BountyBoard::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APW_BountyBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

