// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PW_HighlightComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_HighlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UPW_HighlightComponent();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void ShowHighlight();

	UFUNCTION(BlueprintCallable)
	void HideHighlight();

private:


	TArray<UMeshComponent*> _meshComps;

	UPROPERTY (EditAnywhere,BlueprintReadWrite, Category = "Gameplay" , meta = (AllowPrivateAccess = "true"))
	TArray<UMeshComponent*> _meshComponentsToIgnore;
};
