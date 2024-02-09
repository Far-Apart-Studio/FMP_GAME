// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PW_TransitionPortal.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE( FSuccessDelegate);

UCLASS()
class PROJECT_WEST_API APW_TransitionPortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APW_TransitionPortal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ToggleActivation( bool value );
private:

	UPROPERTY(VisibleAnywhere , meta = (AllowPrivateAccess = "true"))
	class USceneComponent* _root;

	UPROPERTY(VisibleAnywhere , meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* _mesh;
	
	UPROPERTY (VisibleAnywhere , meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* _extractionBox;

	UPROPERTY( BlueprintReadWrite, VisibleAnywhere  , meta = (AllowPrivateAccess = "true"))
	bool _winCondition;

	UPROPERTY( BlueprintReadWrite, VisibleAnywhere  , meta = (AllowPrivateAccess = "true"))
	bool _isActivated;

	class APW_LobbyGameMode* _lobbyGameMode;

	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = "true"))
	FSuccessDelegate OnSuccess;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FString _mapPath;

	void CheckForOverlap();
};
