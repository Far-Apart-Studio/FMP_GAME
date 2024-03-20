
#include "PW_SideObjectiveManager.h"
#include "PW_SideObjective.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PROJECT_WEST/POI/PW_PoiManager.h"
#include "PROJECT_WEST/DebugMacros.h"

APW_SideObjectiveManager::APW_SideObjectiveManager()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	_numOfObjectivesPerMap = 3;
}

void APW_SideObjectiveManager::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	InitialiseAllObjectives();
}

void APW_SideObjectiveManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APW_SideObjectiveManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APW_SideObjectiveManager, _activeObjectiveEntries);
	DOREPLIFETIME(APW_SideObjectiveManager, _activeObjectives);
}

void APW_SideObjectiveManager::InitialiseAllObjectives()
{
	_poiManager = Cast<APW_PoiManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APW_PoiManager::StaticClass()));

	if (!_poiManager)
	{
		DEBUG_STRING ("No POI Manager Found");
		return;
	}
	
	TArray<FSideObjectiveData> _allSideObjectiveData = GetAllObjectivesData();

	if (_allSideObjectiveData.Num() == 0)
	{
		DEBUG_STRING ("No Side Objectives Found");
		return;
	}
	
	TArray<FSideObjectiveData> _selectedObjectivesData = TArray<FSideObjectiveData>();

	for (int i = 0; i < _numOfObjectivesPerMap; i++)
	{
		if (_allSideObjectiveData.Num() == 0)
		{
			break;
		}

		const int32 _randomIndex = FMath::RandRange(0, _allSideObjectiveData.Num() - 1);
		_selectedObjectivesData.Add(_allSideObjectiveData[_randomIndex]);
		_allSideObjectiveData.RemoveAt(_randomIndex);
	}

	for (const FSideObjectiveData& sideObjectiveData : _selectedObjectivesData)
	{
		if (sideObjectiveData._sideObjectivesInfo.Num() == 0)
		{
			continue;
		}
		
		const int32 _randomIndex = FMath::RandRange(0, sideObjectiveData._sideObjectivesInfo.Num() - 1);
		_selectedObjectiveEntries.Add({sideObjectiveData._taskType, sideObjectiveData._sideObjectivesInfo[_randomIndex]});
	}

	SortAvailableObjectives();

	for (const FSideObjectiveEntry& sideObjectiveEntry : _selectedObjectiveEntries)
	{
		if (APW_SideObjective* _objective = InitialiseObjective(sideObjectiveEntry))
		{
			_activeObjectives.Add(_objective);
		}
	}
}

APW_SideObjective* APW_SideObjectiveManager::InitialiseObjective(const FSideObjectiveEntry& sideObjectiveData)
{
	APW_SideObjective* objective  = nullptr;
	const TSubclassOf<class APW_SideObjective> _objectiveClassType = GetClassType(sideObjectiveData._taskType);

	if(APW_PoiArea* _poiArea = _poiManager->GetPOIWithID(sideObjectiveData._sideObjectiveInfo._poiID))
	{
		objective = Cast<APW_SideObjective>(GetWorld()->SpawnActor(_objectiveClassType));
		objective->_onObjectiveStatChanged.AddDynamic(this, &APW_SideObjectiveManager::OnObjectiveStateChanged);
		objective->SetUp(sideObjectiveData, _poiArea);
		_activeObjectiveEntries.Add(sideObjectiveData);
	}

	return objective;
}

void APW_SideObjectiveManager::OnObjectiveStateChanged(APW_SideObjective* objective)
{
	objective->GetObjectiveState() == EObjectiveState::ECompleted ? OnObjectiveCompleted(objective) : OnObjectiveFailed(objective);
}

void APW_SideObjectiveManager::OnObjectiveCompleted(APW_SideObjective* ComplectedObjective)
{
	//DEBUG_STRING ("Objective Completed");
	//ComplectedObjective->Destroy();
	_onObjectiveCompleted.Broadcast(ComplectedObjective);
}

void APW_SideObjectiveManager::OnObjectiveFailed(APW_SideObjective* FailedObjective)
{
	//DEBUG_STRING ("Objective Failed");
	//FailedObjective->Destroy();
	_onObjectiveFailed.Broadcast(FailedObjective);
}

TArray<FSideObjectiveData> APW_SideObjectiveManager::GetAllObjectivesData() const
{
	TArray<FSideObjectiveData> _sideObjectiveData;
	if (_ItemDataTable)
	{
		if (const FMapObjectives* itemData = _ItemDataTable->FindRow<FMapObjectives>(*_mapID, ""))
		{
			_sideObjectiveData = itemData->_sideObjectiveData;
		}
	}
	return _sideObjectiveData;
}

TSubclassOf<APW_SideObjective> APW_SideObjectiveManager::GetClassType(ETaskType taskType)
{
	for (const FObjectiveClassData objectiveClass : _objectiveClasses)
	{
		if (objectiveClass._taskType == taskType)
		{
			return objectiveClass._objectiveClassType;
		}
	}
	return nullptr;
}

void APW_SideObjectiveManager::SortAvailableObjectives()
{
	_selectedObjectiveEntries.Sort([](const FSideObjectiveEntry& A, const FSideObjectiveEntry& B)
	{
		return !A._sideObjectiveInfo._poiID.IsEmpty() && B._sideObjectiveInfo._poiID.IsEmpty();
	});
}
