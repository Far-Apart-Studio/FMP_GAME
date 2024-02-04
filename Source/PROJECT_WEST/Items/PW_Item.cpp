// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_Item.h"
#include "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/PlayerController/PW_PlayerController.h"
#include "PROJECT_WEST/PW_ItemHandlerComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathLibrary.h"
#include "PROJECT_WEST/DebugMacros.h"

// Sets default values
APW_Item::APW_Item()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	_itemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(_itemMesh);
	_itemMesh->SetIsReplicated(true);

	_itemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	_itemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	_itemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	EnableCustomDepth(true);
	//_itemMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	//_itemMesh->MarkRenderStateDirty();

	_areaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	_areaSphere->SetupAttachment(RootComponent);
	_areaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	_areaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	_pickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	_pickupWidget->SetupAttachment(RootComponent);

	_isVisible = true;
}

// Called when the game starts or when spawned
void APW_Item::BeginPlay()
{
	Super::BeginPlay();

	_areaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	_areaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	_areaSphere->OnComponentBeginOverlap.AddDynamic(this, &APW_Item::OnSphereOverlap);
	_areaSphere->OnComponentEndOverlap.AddDynamic(this, &APW_Item::OnSphereEndOverlap);

	if (_pickupWidget)
	{
		_pickupWidget->SetVisibility(false);
	}

	SetItemState(EItemState::EIS_Dropped);
}

// Called every frame
void APW_Item::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APW_Item::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APW_Item, _itemState);
	DOREPLIFETIME_CONDITION(APW_Item, _bUseServerSideRewind, COND_OwnerOnly);
	DOREPLIFETIME(APW_Item, _isVisible);
}

void APW_Item::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APW_Character* characterController = Cast<APW_Character>(OtherActor);
	if (characterController)
	{
		UPW_ItemHandlerComponent * itemHandler =  Cast<UPW_ItemHandlerComponent>(characterController->GetComponentByClass(UPW_ItemHandlerComponent::StaticClass()));
		if (itemHandler)
		{
			itemHandler->SetOverlappingItem(this);
		}
	}
}

void APW_Item::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APW_Character* characterController = Cast<APW_Character>(OtherActor);
	if (characterController)
	{
		UPW_ItemHandlerComponent * itemHandler =  Cast<UPW_ItemHandlerComponent>(characterController->GetComponentByClass(UPW_ItemHandlerComponent::StaticClass()));
		if (itemHandler)
		{
			itemHandler->SetOverlappingItem(nullptr);
		}
	}
}

void APW_Item::OnItemStateSet()
{
	switch (_itemState)
	{
		case EItemState::EIS_Pickup:
			OnPicked();
			break;
		case EItemState::EIS_Dropped:
			OnDropped();
			break;
	}
}

void APW_Item::OnPicked()
{
	ShowPickupWidget(false);
	
	_areaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_itemMesh->SetSimulatePhysics(false);
	_itemMesh->SetEnableGravity(false);
	_itemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	EnableCustomDepth(false);

	_ownerCharacter = _ownerCharacter == nullptr ? Cast<APW_Character>(GetOwner()) : _ownerCharacter;
	if (_ownerCharacter && _bUseServerSideRewind)
	{
		_ownerPlayerController = _ownerPlayerController == nullptr ? Cast<APW_PlayerController>(_ownerCharacter->Controller) : _ownerPlayerController;
		if (_ownerPlayerController && HasAuthority() && !_ownerPlayerController->HighPingDelegate.IsBound())
		{
			_ownerPlayerController->HighPingDelegate.AddDynamic(this, &APW_Item::OnPingTooHigh);
		}
	}
}

void APW_Item::OnDropped()
{
	_areaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	_itemMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	
	_itemMesh->SetSimulatePhysics(true);
	_itemMesh->SetEnableGravity(true);
	_itemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	_itemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	_itemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	_itemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	//_itemMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	_itemMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);

	_ownerCharacter = _ownerCharacter == nullptr ? Cast<APW_Character>(GetOwner()) : _ownerCharacter;
	if (_ownerCharacter)
	{
		_ownerPlayerController = _ownerPlayerController == nullptr ? Cast<APW_PlayerController>(_ownerCharacter->Controller) : _ownerPlayerController;
		if (_ownerPlayerController && HasAuthority() && _ownerPlayerController->HighPingDelegate.IsBound())
		{
			_ownerPlayerController->HighPingDelegate.AddDynamic(this, &APW_Item::OnPingTooHigh);
		}
	}

	_ownerCharacter = nullptr;
	_ownerPlayerController = nullptr;
}

void APW_Item::OnPingTooHigh(bool bPingTooHigh)
{
	_bUseServerSideRewind = !bPingTooHigh;
}

void APW_Item::OnRep_Owner()
{
	Super::OnRep_Owner();
	
	if (Owner == nullptr)
	{
		_ownerCharacter = nullptr;
		_ownerPlayerController = nullptr;
	}
	else
	{
		_ownerCharacter = _ownerCharacter == nullptr ? Cast<APW_Character>(Owner) : _ownerCharacter;
		_ownerPlayerController = _ownerPlayerController == nullptr ? Cast<APW_PlayerController>(_ownerCharacter->Controller) : _ownerPlayerController;
	}
}

void APW_Item::OnVisibilityChange(bool bIsVisible)
{
	if(_itemMesh)
		_itemMesh->SetVisibility(bIsVisible);
}

void APW_Item::ShowPickupWidget(bool bShowWidget)
{
	if (_pickupWidget)
		_pickupWidget->SetVisibility(bShowWidget);
}

void APW_Item::EnableCustomDepth(bool bEnable)
{
	if(_itemMesh)
		_itemMesh->SetRenderCustomDepth(bEnable);
}

void APW_Item::SetItemState(EItemState State)
{
	_itemState = State;
	OnItemStateSet();
}

void APW_Item::OnRep_ItemState()
{
	//DEBUG_STRING("OnRep_ItemState : " + FString::FromInt((int)_itemState));
	OnItemStateSet();
}

void APW_Item::OnRep_VisibilityChange()
{
	//DEBUG_STRING( "OnRep_VisibilityChange : " + FString::FromInt(_isVisible) );
	OnVisibilityChange(_isVisible);
}
