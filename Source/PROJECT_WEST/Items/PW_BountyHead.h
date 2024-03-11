// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PROJECT_WEST/PW_ItemObject.h"
#include "PW_BountyHead.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API APW_BountyHead : public APW_ItemObject
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lantern", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* _skeletalMesh;

public:
	APW_BountyHead();
	
};
