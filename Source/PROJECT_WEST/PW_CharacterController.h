// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PW_CharacterController.generated.h"

UCLASS()
class PROJECT_WEST_API APW_CharacterController : public ACharacter
{
	GENERATED_BODY()


private:
	UPROPERTY(EditAnywhere, Category = "Character")
	class UCameraComponent* _cameraComponent;

public:
	APW_CharacterController();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;
	void MoveForward(float value);
	void MoveRight(float value);
	void LookRight(float value);
	void LookUp(float value);
};
