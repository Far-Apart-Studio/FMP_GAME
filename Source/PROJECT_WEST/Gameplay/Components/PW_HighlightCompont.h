// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PW_HighlightCompont.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_HighlightCompont : public UActorComponent
{
	GENERATED_BODY()

public:	

	UPW_HighlightCompont();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ShowHighlight();
	void HideHighlight();

private:
	
	TArray<UStaticMeshComponent*> _meshComps;
	TArray<USkeletalMeshComponent*> _skeletalMeshComps;
		
};
