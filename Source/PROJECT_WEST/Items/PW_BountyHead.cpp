// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_BountyHead.h"

APW_BountyHead::APW_BountyHead()
{
	_skeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	_skeletalMesh->SetupAttachment(_itemCollisionMesh);
	_skeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_skeletalMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	_skeletalMesh->SetIsReplicated(true);
}
