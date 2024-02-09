// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_BountyBoard.h"

#include "BountySystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/GameModes/PW_LobbyGameMode.h"
#include "PROJECT_WEST/Gameplay/PW_GameInstance.h"


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

	if(HasAuthority())
	{
		_triggerBox->OnComponentBeginOverlap.AddDynamic(this, &APW_BountyBoard::OnOverlapBegin);
		_triggerBox->OnComponentEndOverlap.AddDynamic(this, &APW_BountyBoard::OnOverlapEnd);
	}
}

// Called every frame
void APW_BountyBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//DetectOverlap();
}

void APW_BountyBoard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APW_BountyBoard, _bountyDataList);
}

void APW_BountyBoard::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndexType, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;

	APW_Character* characterController = Cast<APW_Character>(OtherActor);
	if (characterController && characterController->IsLocallyControlled())
	{
		DEBUG_STRING( "OnOverlapBegin Player");
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
	if (!HasAuthority()) return;
	
	APW_Character* characterController = Cast<APW_Character>(OtherActor);
	if (characterController && characterController->IsLocallyControlled())
	{
		DEBUG_STRING( " OnOverlapEnd Player");
		APlayerController* playerController = characterController->GetController<APlayerController>();
		if (playerController)
		{
			playerController->bShowMouseCursor = false;
			playerController->SetInputMode(FInputModeGameOnly());
		}
	}
}

void APW_BountyBoard::OnRep_BountyListChanged()
{
	_bountyDataListChanged.Broadcast(_bountyDataList);
}

void APW_BountyBoard::PopulateBountyDataList()
{
	UPW_GameInstance* gameInstance = Cast<UPW_GameInstance>(GetGameInstance());
	if ( gameInstance )
	{
		if ( gameInstance->GetGameSessionData()._bountyDataList.Num() > 0 )
		{
			_bountyDataList = gameInstance->GetGameSessionData()._bountyDataList;
			_bountyDataListChanged.Broadcast(_bountyDataList);

			DEBUG_STRING( "PopulateBountyDataList from GameInstance");
			return;
		}
	}
	
	APW_LobbyGameMode* gameMode = GetWorld()->GetAuthGameMode<APW_LobbyGameMode>();
	if(gameMode)
	{
		TArray<EBountyDifficulty> difficulties;
		difficulties.Add(EBountyDifficulty::OneStar);
		difficulties.Add(EBountyDifficulty::TwoStar);
		difficulties.Add(EBountyDifficulty::ThreeStar);
		_bountyDataList = gameMode->GetBountySystemComponent()->GetBountyDataList(difficulties);

		if ( gameInstance )
		{
			gameInstance->GetGameSessionData()._bountyDataList = _bountyDataList;
		}
		_bountyDataListChanged.Broadcast(_bountyDataList);
	}
}

void APW_BountyBoard::DetectOverlap()
{
	if (!HasAuthority()) return;
	
	if(_isOverlapping)
	{
		return;
	}
	
	TArray<AActor*> overlappingActors;
	_triggerBox->GetOverlappingActors(overlappingActors, APW_Character::StaticClass());

	if (overlappingActors.Num() > 0)
	{
		_isOverlapping = true;
		APW_Character* characterController = Cast<APW_Character>(overlappingActors[0]);
		if (characterController)
		{
			DEBUG_STRING( " Overlap Begin Player");
			APlayerController* playerController = characterController->GetController<APlayerController>();
			playerController->bShowMouseCursor = true;
		}
	}
}

