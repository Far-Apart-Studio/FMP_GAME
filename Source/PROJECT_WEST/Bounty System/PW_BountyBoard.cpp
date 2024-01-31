// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_BountyBoard.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "PROJECT_WEST/PW_CharacterController.h"
#include "PROJECT_WEST/DebugMacros.h"

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

	_triggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	_triggerBox->SetupAttachment(_root);
	_triggerBox->SetIsReplicated(true);
	_triggerBox->SetCollisionProfileName(FName("OverlapAllDynamic"));
}

void APW_BountyBoard::BeginPlay()
{
	Super::BeginPlay();
	_triggerBox->OnComponentBeginOverlap.AddDynamic(this, &APW_BountyBoard::OnOverlapBegin);
	_triggerBox->OnComponentEndOverlap.AddDynamic(this, &APW_BountyBoard::OnOverlapEnd);
}

// Called every frame
void APW_BountyBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//DetectOverlap();
}

void APW_BountyBoard::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndexType, bool bFromSweep, const FHitResult& SweepResult)
{
	APW_CharacterController* characterController = Cast<APW_CharacterController>(OtherActor);
	if (characterController)
	{
		PrintString( "OnOverlapBegin Player");
		APlayerController* playerController = characterController->GetController<APlayerController>();
		if (playerController)
		{
			playerController->bShowMouseCursor = true;
			playerController->SetInputMode(FInputModeGameAndUI());	
		}
	}
}

void APW_BountyBoard::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndexType)
{
	APW_CharacterController* characterController = Cast<APW_CharacterController>(OtherActor);
	if (characterController)
	{
		PrintString( " OnOverlapEnd Player");
		APlayerController* playerController = characterController->GetController<APlayerController>();
		if (playerController)
		{
			playerController->bShowMouseCursor = false;
			playerController->SetInputMode(FInputModeGameOnly());
		}
	}
}

void APW_BountyBoard::DetectOverlap()
{
	if(_isOverlapping)
	{
		return;
	}
	
	TArray<AActor*> overlappingActors;
	_triggerBox->GetOverlappingActors(overlappingActors, APW_CharacterController::StaticClass());

	if (overlappingActors.Num() > 0)
	{
		_isOverlapping = true;
		APW_CharacterController* characterController = Cast<APW_CharacterController>(overlappingActors[0]);
		if (characterController)
		{
			PrintString( " Overlap Begin Player");
			APlayerController* playerController = characterController->GetController<APlayerController>();
			playerController->bShowMouseCursor = true;
		}
	}
}

