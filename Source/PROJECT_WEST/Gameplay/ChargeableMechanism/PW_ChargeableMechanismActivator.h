// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PW_ChargeableMechanismActivator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCompletedDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_ChargeableMechanismActivator : public UActorComponent
{
	GENERATED_BODY()

public:	

	UPW_ChargeableMechanismActivator();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable)
	FOnCompletedDelegate _OnAllChargeCompleted;

private:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess = "true"))
	TArray<class APW_ChargeableMechanism*> _chargeableMechanisList;

	int _complctedChargeableMechanisms = 0;
	
	void InitializeChargeableMechanisms();

	UFUNCTION()
	void OnChargeCompleted();
};
