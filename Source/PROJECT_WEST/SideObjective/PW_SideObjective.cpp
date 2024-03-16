
#include "PW_SideObjective.h"

#include "Net/UnrealNetwork.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/POI/PW_PoiArea.h"

APW_SideObjective::APW_SideObjective()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	_objectiveState = EObjectiveState::ENone;
}

void APW_SideObjective::BeginPlay()
{
	Super::BeginPlay();
}

void APW_SideObjective::HandleTimer(float deltaTime)
{
	if (_startTimer && _objectiveState == EObjectiveState::ENone)
	{
		_currentObjectiveTime -= deltaTime;
		if (_currentObjectiveTime <= 0)
		{
			_startTimer = false;
			Failed();
		}

		//DEBUG_STRING("Time: " + GetElapsedTime());
	}
}

void APW_SideObjective::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
		HandleTimer (DeltaTime); 
}

void APW_SideObjective::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APW_SideObjective, _objectiveData);
	DOREPLIFETIME(APW_SideObjective, _currentObjectiveAmount);
	DOREPLIFETIME(APW_SideObjective, _currentObjectiveTime);
	DOREPLIFETIME(APW_SideObjective, _objectiveState);
}

void APW_SideObjective::SetUp(FSideObjectiveEntry sideObjectiveData, APW_PoiArea* poiArea)
{
	if (!HasAuthority()) return;
	
	_poiArea = poiArea;
	_objectiveData = sideObjectiveData;
	_currentObjectiveTime = sideObjectiveData._sideObjectiveInfo._objectiveDuration;

	if (_currentObjectiveTime > 0)
	{
		_poiArea->OnPoiTriggered.AddDynamic(this, &APW_SideObjective::OnPOITriggered);
	}
}

void APW_SideObjective::Completed()
{
	_objectiveState = EObjectiveState::ECompleted;
	_onObjectiveStatChanged.Broadcast(this);
}

void APW_SideObjective::Failed()
{
	_objectiveState = EObjectiveState::EFailed;
	_onObjectiveStatChanged.Broadcast(this);
}

void APW_SideObjective::OnPOITriggered(APW_PoiArea* Poi)
{
	_startTimer = true;
}

void APW_SideObjective::OnRep_Complected()
{
	_onObjectiveStatChanged.Broadcast(this);
}

FString APW_SideObjective::GetElapsedTime()
{
	return ConvertToTime(_currentObjectiveTime);
}

FString APW_SideObjective::ConvertToTime(float time)
{
	const int32 minutes = FMath::FloorToInt(time / 60);
	const int32 seconds = time - (minutes * 60);
	FString timeString = FString::Printf(TEXT("%02d:%02d"), minutes, seconds);
	return timeString;
}

