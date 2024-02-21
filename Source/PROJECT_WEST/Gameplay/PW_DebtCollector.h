// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PROJECT_WEST/Interfaces//PW_InteractableInterface.h"
#include "PW_DebtCollector.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract,bool, success);

UCLASS()
class PROJECT_WEST_API APW_DebtCollector : public ACharacter, public IPW_InteractableInterface
{
	GENERATED_BODY()

public:

	APW_DebtCollector();

protected:

	virtual void BeginPlay() override;
	
	virtual void StartFocus_Implementation() override;
	virtual void EndFocus_Implementation() override;
	virtual void StartInteract_Implementation(AActor* owner) override;
	virtual void EndInteract_Implementation() override;
	virtual bool IsInteracting_Implementation() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int32 _debtMinStartAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int32 __debtMaxStartAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int32 _debtIncreaseAmount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int32 _debtAmount;

public:	

	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetDebtAmount(int32 day);

	FOnInteract _onInteract;

	void HandleBounty();

	UFUNCTION(Server, Reliable)
	void ServerHandleBounty();
	
	void LocalHandleBounty();
};
