
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
	TArray<FSideObjectiveData> _sideObjectives;

};
