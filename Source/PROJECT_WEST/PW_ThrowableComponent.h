// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW_Character.h"
#include "Components/ActorComponent.h"
#include "PW_ThrowableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FThrowDelegate);

class APW_ItemObject;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_ThrowableComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Throwable")
	float _throwForce = 1000.0f;
	
	UPROPERTY(VisibleAnywhere, Category = "Throwable")
	APW_ItemObject* _throwableItemObject;

public:
	
	UPROPERTY(BlueprintAssignable, Category = "Throwable")
	FThrowDelegate OnQueueThrow;
	
	UPROPERTY(BlueprintAssignable, Category = "Throwable")
	FThrowDelegate OnCancelThrow;
	
	UPROPERTY(BlueprintAssignable, Category = "Throwable")
	FThrowDelegate OnThrow;

public:	
	UPW_ThrowableComponent();
	void InitialiseComponent();
	void QueueThrow();
	void CancelThrow();
	void Throw();
	void SetThrowableInputBinding(APW_Character* characterOwner);
	void RemoveThrowableInputBinding(APW_Character* characterOwner);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
