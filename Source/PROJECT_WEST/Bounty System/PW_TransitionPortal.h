// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PW_TransitionPortal.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FActivationChangedDelegate, bool , IsActivated );
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FSuccessDelegate);

UCLASS()
class PROJECT_WEST_API APW_TransitionPortal : public AActor
{
	GENERATED_BODY()
	
public:	

	APW_TransitionPortal();

protected:

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:	

	virtual void Tick(float DeltaTime) override;

	UFUNCTION (BlueprintCallable)
	void ToggleActivation( bool value );

	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = "true"))
	FSuccessDelegate OnSuccess;
	
private:

	UPROPERTY (VisibleAnywhere , meta = (AllowPrivateAccess = "true"))
	class USceneComponent* _root;

	UPROPERTY (VisibleAnywhere ,BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* _mesh;
	
	UPROPERTY (VisibleAnywhere ,BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* _extractionBox;

	UPROPERTY (BlueprintReadWrite, VisibleAnywhere  , meta = (AllowPrivateAccess = "true"))
	bool _winCondition;

	UPROPERTY(ReplicatedUsing = OnRep_ActivationChanged, BlueprintReadWrite, VisibleAnywhere  , meta = (AllowPrivateAccess = "true"))
	bool _isActivated;

	class APW_LobbyGameMode* _lobbyGameMode;
	
	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = "true"))
	FActivationChangedDelegate OnActivationChangedDelegate;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FString _mapPath;

	void CheckForOverlap();

	UFUNCTION()
	void OnRep_ActivationChanged();
};
