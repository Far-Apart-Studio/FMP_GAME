// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PW_MaterialEffectComponent.generated.h"

UENUM()
enum class EEffectDirection: uint8
{
	ED_Forward UMETA(DisplayName = "Foward"),
	ED_Backward UMETA(DisplayName = "Backward")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_MaterialEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UPW_MaterialEffectComponent();

protected:

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	
	UPROPERTY (EditAnywhere, Category = "Gameplay" , meta = (AllowPrivateAccess = "true"))
	FName _effectName;

	UPROPERTY (EditAnywhere, Category = "Gameplay" , meta = (AllowPrivateAccess = "true"))
	float _startingEffectValue;
	
	UPROPERTY (EditAnywhere, Category = "Gameplay" , meta = (AllowPrivateAccess = "true"))
	float _fadeSpeed;

	UPROPERTY (EditAnywhere, Category = "Gameplay" , meta = (AllowPrivateAccess = "true"))
	EEffectDirection _direction;

	UPROPERTY (EditAnywhere, Category = "Gameplay" , meta = (AllowPrivateAccess = "true"))
	bool _autoGenerateMeshComponents;

	UPROPERTY (ReplicatedUsing= OnRep_EffectValueChanged, VisibleAnywhere, Category = "Gameplay" , meta = (AllowPrivateAccess = "true"))
	float _currentEffectValue;

	UPROPERTY (VisibleAnywhere, Category = "Gameplay" , meta = (AllowPrivateAccess = "true"))
	bool _isEffectActive;
	
	UPROPERTY (EditAnywhere, Category = "Gameplay" , meta = (AllowPrivateAccess = "true"))
	TArray<USkeletalMeshComponent*> _skeletalMeshComponents;

	UPROPERTY (EditAnywhere, Category = "Gameplay" , meta = (AllowPrivateAccess = "true"))
	TArray<UStaticMeshComponent*> _staticMeshComponents;

private:
	void GenerateMeshComponents();
	void HandleEffect(float DeltaTime);
	void SetMeshComponentsEffectValue(float value);

	UFUNCTION()
	void OnRep_EffectValueChanged();

public:

	UFUNCTION(BlueprintCallable)
	void ActivateEffect(EEffectDirection direction);
};
