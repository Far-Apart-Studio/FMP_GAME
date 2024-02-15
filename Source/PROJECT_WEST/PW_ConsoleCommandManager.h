// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PW_ConsoleCommandManager.generated.h"

class UPW_ConsoleCommand;

UCLASS( BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_ConsoleCommandManager : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Console Command Manager")
	TArray<TSubclassOf<UPW_ConsoleCommand>> _consoleCommandsClasses;

public:
	UPROPERTY(Transient, Replicated)
	TArray<UPW_ConsoleCommand*> _consoleCommandsInstances;

public:	
	UPW_ConsoleCommandManager();
	virtual bool ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor) override;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
