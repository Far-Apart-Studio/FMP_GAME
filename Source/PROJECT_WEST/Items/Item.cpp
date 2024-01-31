// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/PlayerController/PW_PlayerController.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	_itemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(_itemMesh);

	_itemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	//_itemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
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
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	_areaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	_areaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	_areaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	_areaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	if (_pickupWidget)
	{
		_pickupWidget->SetVisibility(false);
	}
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItem, _itemState);
	DOREPLIFETIME_CONDITION(AItem, _bUseServerSideRewind, COND_OwnerOnly);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APW_Character* characterController = Cast<APW_Character>(OtherActor);
	if (characterController)
	{
		characterController->SetOverlappingItem(this);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APW_Character* characterController = Cast<APW_Character>(OtherActor);
	if (characterController)
	{
		characterController->SetOverlappingItem(nullptr);
	}
}

void AItem::OnItemStateSet()
{
	switch (_itemState)
	{
	case EItemState::EIS_Equipped:
		OnEquipped();
		break;
	case EItemState::EIS_Dropped:
		OnDropped();
		break;
	}
}

void AItem::OnEquipped()
{
	ShowPickupWidget(false);
	_areaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_itemMesh->SetSimulatePhysics(false);
	_itemMesh->SetEnableGravity(false);
	_itemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//_itemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//_itemMesh->SetEnableGravity(true);
	//_itemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	
	EnableCustomDepth(false);

	_ownerCharacter = _ownerCharacter == nullptr ? Cast<APW_Character>(GetOwner()) : _ownerCharacter;
	if (_ownerCharacter && _bUseServerSideRewind)
	{
		_ownerPlayerController = _ownerPlayerController == nullptr ? Cast<APW_PlayerController>(_ownerCharacter->Controller) : _ownerPlayerController;
		if (_ownerPlayerController && HasAuthority() && !_ownerPlayerController->HighPingDelegate.IsBound())
		{
			_ownerPlayerController->HighPingDelegate.AddDynamic(this, &AItem::OnPingTooHigh);
		}
	}
}

void AItem::OnDropped()
{
	if (HasAuthority())
	{
		_areaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	
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
			_ownerPlayerController->HighPingDelegate.AddDynamic(this, &AItem::OnPingTooHigh);
		}
	}
}

void AItem::OnPingTooHigh(bool bPingTooHigh)
{
	_bUseServerSideRewind = !bPingTooHigh;
}

void AItem::OnRep_Owner()
{
	Super::OnRep_Owner();
	
	Super::OnRep_Owner();
	if (Owner == nullptr)
	{
		_ownerCharacter = nullptr;
		_ownerPlayerController = nullptr;
	}
	else
	{
		_ownerCharacter = _ownerCharacter == nullptr ? Cast<APW_Character>(Owner) : _ownerCharacter;
		//if (_OwnerCharacter && _OwnerCharacter->GetEquippedWeapon() && _OwnerCharacter->GetEquippedWeapon() == this)
		//{
		//	SetHUDAmmo();
		//}
	}
}

void AItem::Dropped()
{
	SetItemState(EItemState::EIS_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	_itemMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	_ownerCharacter = nullptr;
	_ownerPlayerController = nullptr;
}

void AItem::ShowPickupWidget(bool bShowWidget)
{
	if (_pickupWidget)
	{
		_pickupWidget->SetVisibility(bShowWidget);
	}
}

void AItem::EnableCustomDepth(bool bEnable)
{
	if(_itemMesh)
	{
		_itemMesh->SetRenderCustomDepth(bEnable);
	}
}

void AItem::SetItemState(EItemState State)
{
	_itemState = State;
	OnItemStateSet();
}

void AItem::OnRep_ItemState()
{
	OnItemStateSet();
}
