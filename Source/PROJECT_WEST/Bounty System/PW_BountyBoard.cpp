// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_BountyBoard.h"

#include "BountySystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/GameModes/PW_LobbyGameMode.h"
#include "PROJECT_WEST/Gameplay/PW_GameInstance.h"
#include "Camera/CameraComponent.h"

APW_BountyBoard::APW_BountyBoard()
{
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

	_cameraPosition = CreateDefaultSubobject<USceneComponent>(TEXT("CameraPosition"));
	_cameraPosition->SetupAttachment(_root);
	_cameraPosition->SetRelativeLocation(FVector(0, 0, 100));

	_cameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	_cameraComponent->SetupAttachment(_cameraPosition);
}

void APW_BountyBoard::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority())
	{

	}
}

void APW_BountyBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APW_BountyBoard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APW_BountyBoard, _bountyDataList);
}

void APW_BountyBoard::StartFocus_Implementation()
{
	ToggleHighlight (true);
}

void APW_BountyBoard::EndFocus_Implementation()
{
	ToggleHighlight (false);
}

void APW_BountyBoard::EndInteract_Implementation()
{
	APW_Character* characterController = Cast<APW_Character>(_character);
	if (characterController && characterController->IsLocallyControlled())
	{
		APlayerController* playerController = characterController->GetController<APlayerController>();
		if (playerController)
		{
			playerController->bShowMouseCursor = false;
			playerController->SetInputMode(FInputModeGameOnly());
			characterController->ToggleMovement(true);
			playerController->SetViewTargetWithBlend(characterController, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic);
		}
	}

	_character = nullptr;
}

bool APW_BountyBoard::IsInteracting_Implementation()
{
	return _character != nullptr;
}

void APW_BountyBoard::StartInteract_Implementation(AActor* owner)
{
	APW_Character* characterController = Cast<APW_Character>(owner);
	if (characterController && characterController->IsLocallyControlled())
	{
		_character = characterController;
		APlayerController* playerController = characterController->GetController<APlayerController>();
		if (playerController)
		{
			playerController->bShowMouseCursor = true;
			playerController->SetInputMode(FInputModeGameAndUI());
			characterController->ToggleMovement(false);
			playerController->SetViewTargetWithBlend(this, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic);
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

		if (gameInstance)
		{
			gameInstance->GetGameSessionData()._bountyDataList = _bountyDataList;
		}
		_bountyDataListChanged.Broadcast(_bountyDataList);
	}
}

void APW_BountyBoard::ToggleHighlight(bool status)
{
	_bountyBoardMesh->bRenderCustomDepth = status;
}
