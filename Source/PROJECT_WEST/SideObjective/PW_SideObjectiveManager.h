
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PROJECT_WEST/SideObjective/SideObjective.h"
#include "PW_SideObjectiveManager.generated.h"

UCLASS()
class PROJECT_WEST_API APW_SideObjectiveManager : public AActor
{
	GENERATED_BODY()
	
public:	

	APW_SideObjectiveManager();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	FString _mapID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	UDataTable* _ItemDataTable;
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	int32 _numOfObjectivesPerMap;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	TArray<FObjectiveClassData> _objectiveClasses;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	TArray<FSideObjectiveEntry> _selectedObjectiveEntries;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	int32 _numOfObjectivesCompleted;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	TArray<APW_SideObjective*> _activeObjectives;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	class APW_PoiManager* _poiManager;

private:

	void InitialiseAllObjectives();
	APW_SideObjective*  InitialiseObjective(const FSideObjectiveEntry& sideObjectiveData);

	UFUNCTION()
	void OnObjectiveCompleted(APW_SideObjective* ComplectedObjective);

	UFUNCTION()
	void OnObjectiveFailed(APW_SideObjective* FailedObjective);

	TArray<FSideObjectiveData> GetAllObjectivesData() const;

	TSubclassOf<class APW_SideObjective> GetClassType (ETaskType taskType);

	void SortAvailableObjectives();
};
