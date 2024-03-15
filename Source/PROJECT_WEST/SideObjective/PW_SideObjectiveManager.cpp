
#include "PW_SideObjectiveManager.h"
#include "PW_SideObjective.h"
#include "Kismet/GameplayStatics.h"
#include "PROJECT_WEST/POI/PW_PoiManager.h"
#include "PROJECT_WEST/DebugMacros.h"

APW_SideObjectiveManager::APW_SideObjectiveManager()
{
	PrimaryActorTick.bCanEverTick = true;

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
	
	TArray<FSideObjectiveData> _sideObjectiveData;
	if (_ItemDataTable)
	{
		if (const FMapObjective* itemData = _ItemDataTable->FindRow<FMapObjective>(*_mapID, ""))
		{
			_sideObjectiveData = itemData->_sideObjectiveData;
		}
	}

	if (_sideObjectiveData.Num() == 0)
	{
		DEBUG_STRING ("No Side Objectives Found");
		return;
	}

	if (APW_SideObjective* objective = InitialiseObjective(_sideObjectiveData[0]))
	{
		_activeObjectives.Add(objective);
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

	DEBUG_STRING ("Objective Initialised");

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

