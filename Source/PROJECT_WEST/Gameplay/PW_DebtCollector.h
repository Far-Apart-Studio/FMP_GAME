// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PW_DebtCollector.generated.h"

UCLASS()
class PROJECT_WEST_API APW_DebtCollector : public ACharacter
{
	GENERATED_BODY()

public:

	APW_DebtCollector();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	int32 _debtStartAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	int32 _debtIncreaseAmount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Info")
	int32 _debtAmount;

public:	

	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetDebtAmount(int32 day);

};
