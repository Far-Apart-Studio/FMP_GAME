// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PROJECT_WEST/Interfaces//PW_InteractableInterface.h"
#include "PW_DebtCollector.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDebtAmountChanged, int32, DebtAmount);

UCLASS()
class PROJECT_WEST_API APW_DebtCollector : public ACharacter, public IPW_InteractableInterface
{
	GENERATED_BODY()

public:

	APW_DebtCollector();

protected:

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void StartFocus_Implementation(AActor* owner) override;
	virtual void EndFocus_Implementation(AActor* owner) override;
	virtual void StartInteract_Implementation(AActor* owner) override;
	virtual bool HasServerInteraction_Implementation() override;
	virtual void ServerStartInteract_Implementation(AActor* owner) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta = (AllowPrivateAccess = "true"))
	class UPW_HighlightComponent* _highlightComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int32 _debtMinStartAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int32 _debtMaxStartAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int32 _debtMinIncreaseAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int32 _debtMaxIncreaseAmount;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int32 _debtIncreaseValue;

	UPROPERTY(Replicated = OnRep_DebtChanged, VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int32 _debtAmount;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Info", meta = (AllowPrivateAccess = "true"))
	bool _isActivated;

	UPROPERTY (BlueprintAssignable, Category = "Gameplay")
	FOnDebtAmountChanged _onDebtAmountChanged;

	UFUNCTION()
	void OnRep_DebtChanged();

public:	

	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetDebtAmount(int32 day);

	void HandleBounty(AActor* owner);

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable, Category = "Gameplay")
	void OnInteracted(bool status);

	FORCEINLINE int32 GetDebtAmount() const { return _debtAmount; }
};
