
#include "PW_SideObjective.h"

APW_SideObjective::APW_SideObjective()
{
	PrimaryActorTick.bCanEverTick = true;

}

void APW_SideObjective::BeginPlay()
{
	Super::BeginPlay();
	
}

void APW_SideObjective::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APW_SideObjective::SetUp(FSideObjectiveEntry sideObjectiveData, APW_PoiArea* poiArea)
{
	_poiArea = poiArea;
}

void APW_SideObjective::Deactivate()
{
	
}

