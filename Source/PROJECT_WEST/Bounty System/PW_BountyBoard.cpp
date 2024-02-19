// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_BountyBoard.h"

#include "BountySystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/PlayerController/PW_PlayerController.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/GameModes/PW_LobbyGameMode.h"
#include "PROJECT_WEST/Gameplay/PW_GameInstance.h"
#include "Camera/CameraComponent.h"

APW_BountyBoard::APW_BountyBoard()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	_root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(_root);
	
	_bountyBoardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BountyBoardMesh"));
	_bountyBoardMesh->SetupAttachment(_root);

	_bountyBoardWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("BountyBoardWidget"));
	_bountyBoardWidget->SetupAttachment(_root);
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
	
	SetReplicates(true);
	
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
	DOREPLIFETIME(APW_BountyBoard, _bountyVoteData);
	DOREPLIFETIME(APW_BountyBoard, _isActivated);
}

void APW_BountyBoard::StartFocus_Implementation()
{
	if (!_isActivated) return;
	ToggleHighlight (true);
}

void APW_BountyBoard::EndFocus_Implementation()
{
	if (!_isActivated) return;
	ToggleHighlight (false);
}

void APW_BountyBoard::EndInteract_Implementation()
{
	if ( !_character) return;
	APW_Character* characterController = Cast<APW_Character>(_character);
	if (characterController && characterController->IsLocallyControlled())
	{
		APW_PlayerController* playerController = characterController->GetController<APW_PlayerController>();
		if (playerController)
		{
			playerController->SetViewTargetWithBlend(characterController, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic);
			characterController->ToggleMovement(true);
			playerController->bShowMouseCursor = false;
			playerController->SetInputMode(FInputModeGameOnly());
			playerController->ToggleHUDVisibility(true);
			_onBoardClosed.Broadcast();
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
	if (!_isActivated) return;
	
	APW_Character* characterController = Cast<APW_Character>(owner);
	if (characterController && characterController->IsLocallyControlled())
	{
		_character = characterController;
		APW_PlayerController* playerController = characterController->GetController<APW_PlayerController>();
		if (playerController)
		{
			playerController->ToggleHUDVisibility(false);
			playerController->SetViewTargetWithBlend(this, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic);
			characterController->ToggleMovement(false);
			playerController->bShowMouseCursor = true;
			playerController->SetInputMode(FInputModeGameAndUI());
			_onBoardOpened.Broadcast();
		}
	}
}

void APW_BountyBoard::OnRep_BountyListChanged()
{
	_bountyDataListChanged.Broadcast(_bountyDataList);
}

void APW_BountyBoard::OnRep_BountyVoteChanged()
{
	_bountyVoteDataChanged.Broadcast(_bountyVoteData);
}

void APW_BountyBoard::PopulateBountyDataList()
{
	APW_LobbyGameMode* gameMode = GetWorld()->GetAuthGameMode<APW_LobbyGameMode>();
	if (!gameMode) return;

	_bountyDataList.Empty();
	
	UPW_GameInstance* gameInstance = Cast<UPW_GameInstance>(GetGameInstance());
	
	if (gameInstance && gameInstance->GetGameSessionData()._bountyDataList.Num() > 0 )
	{
		_bountyDataList = gameInstance->GetGameSessionData()._bountyDataList;

		FBountyDataEntry newBountyData = gameMode->GetBountySystemComponent()->GetBountyDataEntry(gameInstance->GetGameSessionData()._bountyDataEntry._bountyDifficulty);
		newBountyData.index = gameInstance->GetGameSessionData()._bountyDataEntry.index;
		_bountyDataList[ gameInstance->GetGameSessionData()._bountyDataEntry.index ] = newBountyData;
		_bountyDataListChanged.Broadcast(_bountyDataList);
		PopulateBountyVoteData(_bountyDataList.Num());
		DEBUG_STRING( "PopulateBountyDataList from GameInstance and refresh last bounty data.");
		return;
	}
	
	TArray<EBountyDifficulty> difficulties;
	difficulties.Add(EBountyDifficulty::EBD_OneStar);
	difficulties.Add(EBountyDifficulty::EBD_TwoStar);
	difficulties.Add(EBountyDifficulty::EBD_ThreeStar);
	difficulties.Add(EBountyDifficulty::EBD_FourStar);
	_bountyDataList = gameMode->GetBountySystemComponent()->GetBountyDataList(difficulties);

	if (gameInstance)
	{
		gameInstance->GetGameSessionData()._bountyDataList = _bountyDataList;
	}
	_bountyDataListChanged.Broadcast(_bountyDataList);
	PopulateBountyVoteData(_bountyDataList.Num());
	DEBUG_STRING( "Populate New BountyDataList from GameMode and save to GameInstance.");
}

void APW_BountyBoard::RefereshBountyDataList()
{
	_bountyDataList.Empty();
	
	APW_LobbyGameMode* gameMode = GetWorld()->GetAuthGameMode<APW_LobbyGameMode>();
	if (!gameMode) return;

	UPW_GameInstance* gameInstance = Cast<UPW_GameInstance>(GetGameInstance());
	if (!gameInstance) return;
	
	TArray<EBountyDifficulty> difficulties;
	difficulties.Add(EBountyDifficulty::EBD_OneStar);
	difficulties.Add(EBountyDifficulty::EBD_TwoStar);
	difficulties.Add(EBountyDifficulty::EBD_ThreeStar);
	difficulties.Add(EBountyDifficulty::EBD_FourStar);
	_bountyDataList = gameMode->GetBountySystemComponent()->GetBountyDataList(difficulties);

	if (gameInstance)
	{
		gameInstance->GetGameSessionData()._bountyDataList = _bountyDataList;
	}
	_bountyDataListChanged.Broadcast(_bountyDataList);
	PopulateBountyVoteData(_bountyDataList.Num());
	DEBUG_STRING( "Populate New BountyDataList from GameMode and save to GameInstance.");
}

void APW_BountyBoard::PopulateBountyVoteData(int numberOfBounties)
{
	_bountyVoteData._bountyVoteDataList.Empty();
	for (int i = 0; i < numberOfBounties; i++)
	{
		FBountyVoteDataEntry entry;
		entry._bountyVotes = 0;
		_bountyVoteData._bountyVoteDataList.Add(entry);
	}
}

int32 APW_BountyBoard::GetTotalVotes()
{
	int32 totalVotes = 0;
	for (FBountyVoteDataEntry& entry : _bountyVoteData._bountyVoteDataList)
	{
		totalVotes += entry._bountyVotes;
	}
	return totalVotes;
}

void APW_BountyBoard::AddVoteToBounty(int32 bountyIndex)
{
	_bountyVoteData._bountyVoteDataList[bountyIndex]._bountyVotes++;
	_bountyVoteDataChanged.Broadcast(_bountyVoteData);
}

void APW_BountyBoard::RemoveVoteFromBounty(int32 bountyIndex)
{
	_bountyVoteData._bountyVoteDataList[bountyIndex]._bountyVotes--;
	_bountyVoteDataChanged.Broadcast(_bountyVoteData);
}

int32 APW_BountyBoard::GetBountyIndexWithHighestVotes()
{
	int32 highestVotes = 0;
	int32 highestVotesIndex = 0;
	for (int32 i = 0; i < _bountyVoteData._bountyVoteDataList.Num(); i++)
	{
		if (_bountyVoteData._bountyVoteDataList[i]._bountyVotes > highestVotes)
		{
			highestVotes = _bountyVoteData._bountyVoteDataList[i]._bountyVotes;
			highestVotesIndex = i;
		}
	}
	return highestVotesIndex;
}

FBountyDataEntry APW_BountyBoard::GetBountyWithHighestVotes()
{
	return  _bountyDataList [GetBountyIndexWithHighestVotes()];
}

void APW_BountyBoard::ToggleHighlight(bool status)
{
	_bountyBoardMesh->SetRenderCustomDepth(status);
}
