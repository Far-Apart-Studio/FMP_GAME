// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PROJECT_WEST/Interfaces//PW_InteractableInterface.h"
#include "PW_DebtCollector.generated.h"

UCLASS()
class PROJECT_WEST_API APW_DebtCollector : public ACharacter, public IPW_InteractableInterface
{
	GENERATED_BODY()

public:

	APW_DebtCollector();

protected:

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void StartFocus_Implementation() override;
	virtual void EndFocus_Implementation() override;
	virtual void StartInteract_Implementation(AActor* owner) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess = "true"))
	class UPW_HighlightCompont* _highlightComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int32 _debtMinStartAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int32 _debtMaxStartAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int32 _debtMinIncreaseAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int32 _debtMaxIncreaseAmount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int32 _debtAmount;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Info", meta = (AllowPrivateAccess = "true"))
	bool _isActivated;

public:	

	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetDebtAmount(int32 day);

	void HandleBounty(AActor* owner);

	FORCEINLINE int32 GetDebtAmount() const { return _debtAmount; }
};
