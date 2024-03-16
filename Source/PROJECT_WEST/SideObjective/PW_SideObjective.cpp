
#include "PW_SideObjective.h"

#include "Net/UnrealNetwork.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/POI/PW_PoiArea.h"

APW_SideObjective::APW_SideObjective()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void APW_SideObjective::BeginPlay()
{
	Super::BeginPlay();
	
}

void APW_SideObjective::HandleTimer(float deltaTime)
{
	if (_startTimer)
	{
		_currentObjectiveTime -= deltaTime;
		if (_currentObjectiveTime <= 0)
		{
			_startTimer = false;
			Deactivate();
		}

		DEBUG_STRING("Time: " + GetElapsedTime());
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
	DOREPLIFETIME(APW_SideObjective, _currentObjectiveAmount);
	DOREPLIFETIME(APW_SideObjective, _currentObjectiveTime);
	DOREPLIFETIME(APW_SideObjective, _completed);
}

void APW_SideObjective::SetUp(FSideObjectiveEntry sideObjectiveData, APW_PoiArea* poiArea)
{
	_poiArea = poiArea;
	_objectiveData = sideObjectiveData;
	_currentObjectiveTime = sideObjectiveData._sideObjectiveInfo._objectiveDuration;

	if (_currentObjectiveTime > 0)
	{
		_poiArea->OnPoiTriggered.AddDynamic(this, &APW_SideObjective::OnPOITriggered);
	}
}

void APW_SideObjective::Deactivate()
{
	_onObjectiveFailed.Broadcast(this);
	_completed = false;
}

void APW_SideObjective::OnPOITriggered(APW_PoiArea* Poi)
{
	DEBUG_STRING ("POI Triggered");
	_startTimer = true;
}

void APW_SideObjective::OnRep_Complected()
{
	_completed ? _onObjectiveCompleted.Broadcast(this) : _onObjectiveFailed.Broadcast(this);
}

FString APW_SideObjective::GetElapsedTime()
{
	return  ConvertToTime(_currentObjectiveTime);
}

FString APW_SideObjective::ConvertToTime(float time)
{
	const int32 minutes = FMath::FloorToInt(time / 60);
	const int32 seconds = time - (minutes * 60);
	FString timeString = FString::Printf(TEXT("%02d:%02d"), minutes, seconds);
	return timeString;
}

