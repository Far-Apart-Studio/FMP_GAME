
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PROJECT_WEST/SideObjective/SideObjective.h"
#include "PW_SideObjectiveManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSideObjectiveCompleted, APW_SideObjective*, complectedObjective);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSideObjectiveFailed, APW_SideObjective*, failedObjective);

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
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	UPROPERTY(BlueprintAssignable, Category = "SideObjective")
	FOnSideObjectiveCompleted _onObjectiveCompleted;

	UPROPERTY(BlueprintAssignable, Category = "SideObjective")
	FOnSideObjectiveFailed _onObjectiveFailed;
	
private:

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	FString _mapID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	UDataTable* _ItemDataTable;
	
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	int32 _numOfObjectivesPerMap;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	TArray<FObjectiveClassData> _objectiveClasses;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	TArray<FSideObjectiveEntry> _selectedObjectiveEntries;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	int32 _numOfObjectivesCompleted;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	TArray<FSideObjectiveEntry> _activeObjectiveEntries;
	
	UPROPERTY( Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	TArray<APW_SideObjective*> _activeObjectives;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	class APW_PoiManager* _poiManager;

private:

	void InitialiseAllObjectives();
	APW_SideObjective*  InitialiseObjective(const FSideObjectiveEntry& sideObjectiveData);

	UFUNCTION()
	void OnObjectiveStateChanged(APW_SideObjective* ComplectedObjective);
	
	void OnObjectiveCompleted(APW_SideObjective* ComplectedObjective);
	
	void OnObjectiveFailed(APW_SideObjective* FailedObjective);

	TArray<FSideObjectiveData> GetAllObjectivesData() const;

	TSubclassOf<class APW_SideObjective> GetClassType (ETaskType taskType);

	void SortAvailableObjectives();
};
