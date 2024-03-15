// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PW_DistanceUnloaderComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnLoaded , AActor* , UnloadedActor);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_DistanceUnloaderComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UPW_DistanceUnloaderComponent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void HandleDistanceUnloading();
	void UnLoad();

	UFUNCTION(BlueprintCallable)
	void SetCanUnload(bool status);

	FOnUnLoaded _onUnloaded;
	
private:

	UPROPERTY(EditAnywhere, Category = "Gameplay",meta = (AllowPrivateAccess = "true"))
	bool _canUnload;
	
	UPROPERTY(EditAnywhere, Category = "Gameplay",meta = (AllowPrivateAccess = "true"))
	float _distanceToUnload;

	UPROPERTY(VisibleAnywhere, Category = "Gameplay",meta = (AllowPrivateAccess = "true"))
	bool _isUnLoaded;
};
