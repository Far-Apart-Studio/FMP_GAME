
#include "PW_SideObjectiveManager.h"
#include "PW_SideObjective.h"
#include "Kismet/GameplayStatics.h"
#include "PROJECT_WEST/POI/PW_PoiManager.h"
#include "PROJECT_WEST/DebugMacros.h"

APW_SideObjectiveManager::APW_SideObjectiveManager()
{
	PrimaryActorTick.bCanEverTick = true;
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

void APW_SideObjectiveManager::InitialiseAllObjectives()
{
	_poiManager = Cast<APW_PoiManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APW_PoiManager::StaticClass()));

	if (!_poiManager)
	{
		DEBUG_STRING ("No POI Manager Found");
		return;
	}
	
	TArray<FSideObjectiveData> _sideObjectiveData = GetAllObjectivesData();

	if (_sideObjectiveData.Num() == 0)
	{
		DEBUG_STRING ("No Side Objectives Found");
		return;
	}

	TArray<FSideObjectiveData> _availableObjectivesData = _sideObjectiveData;

	for (int i = 0; i < _numOfObjectivesPerMap; i++)
	{
		if (_availableObjectivesData.Num() == 0)
		{
			DEBUG_STRING ("No More Objectives Available");
			break;
		}

		const int32 _randomIndex = FMath::RandRange(0, _availableObjectivesData.Num() - 1);
		if (DoesObjectiveTypeExist(_availableObjectivesData[_randomIndex]))
		{
			DEBUG_STRING ("Objective Type Already Exists");
			continue;
		}
		
		_selectedObjectivesData.Add(_availableObjectivesData[_randomIndex]);
		_availableObjectivesData.RemoveAt(_randomIndex);
	}

	for (const FSideObjectiveData& sideObjectiveData : _selectedObjectivesData)
	{
		if (APW_SideObjective* objective = InitialiseObjective(sideObjectiveData))
		{
			DEBUG_STRING ("Objective Initialised");
			_activeObjectives.Add(objective);
		}
	}
}

APW_SideObjective* APW_SideObjectiveManager::InitialiseObjective(const FSideObjectiveData& sideObjectiveData)
{
	APW_SideObjective* objective  = nullptr;
	const TSubclassOf<class APW_SideObjective> _objectiveClassType = GetClassType(sideObjectiveData._taskType);

	if(APW_PoiArea* _poiArea = _poiManager->GetPOIWithID(sideObjectiveData._poiID))
	{
		objective = Cast<APW_SideObjective>(GetWorld()->SpawnActor(_objectiveClassType));
		objective->_onObjectiveCompleted.AddDynamic(this, &APW_SideObjectiveManager::OnObjectiveCompleted);
		objective->_onObjectiveFailed.AddDynamic(this, &APW_SideObjectiveManager::OnObjectiveFailed);
		objective->SetUp(sideObjectiveData, _poiArea);
	}

	return objective;
}

void APW_SideObjectiveManager::OnObjectiveCompleted(APW_SideObjective* ComplectedObjective)
{
	DEBUG_STRING ("Objective Completed");
	ComplectedObjective->Destroy();
}

void APW_SideObjectiveManager::OnObjectiveFailed(APW_SideObjective* FailedObjective)
{
	DEBUG_STRING ("Objective Failed");
	FailedObjective->Destroy();
}

TArray<FSideObjectiveData> APW_SideObjectiveManager::GetAllObjectivesData() const
{
	TArray<FSideObjectiveData> _sideObjectiveData;
	if (_ItemDataTable)
	{
		if (const FMapObjective* itemData = _ItemDataTable->FindRow<FMapObjective>(*_mapID, ""))
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

bool APW_SideObjectiveManager::DoesObjectiveTypeExist(const FSideObjectiveData& sideObjectiveData)
{
	for (const FSideObjectiveData objectiveClass : _selectedObjectivesData)
	{
		if (objectiveClass._taskType == sideObjectiveData._taskType)
		{
			return true;
		}
	}
	return false;
}

