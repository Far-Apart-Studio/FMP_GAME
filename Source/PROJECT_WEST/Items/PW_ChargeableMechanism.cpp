#include "PW_ChargeableMechanism.h"

#include "PW_Lantern.h"
#include "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/PW_InventoryHandler.h"
#include "PROJECT_WEST/DebugMacros.h"
APW_ChargeableMechanism::APW_ChargeableMechanism()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	//bAlwaysRelevant = true;
}

void APW_ChargeableMechanism::BeginPlay()
{
	Super::BeginPlay();
	
}

void APW_ChargeableMechanism::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_chargeActivated && HasAuthority())
	{
		HandleCharging(DeltaTime);
	}
}

void APW_ChargeableMechanism::StartFocus_Implementation(AActor* targetActor)
{
	_OnFocusBegin.Broadcast(IsLanternEquipped(targetActor));
}

void APW_ChargeableMechanism::EndFocus_Implementation(AActor* targetActor)
{
	_OnFocusEnd.Broadcast();
	_lantern = nullptr;
}

void APW_ChargeableMechanism::ServerStartInteract_Implementation(AActor* targetActor)
{
	if(!_lantern) return;

	_character = Cast<APW_Character>(targetActor);
	if (!_character)
		return;
	
	_character->OnInteractButtonHeld.AddDynamic(this, &APW_ChargeableMechanism::OnChargeButtonHeld);
}

void APW_ChargeableMechanism::ServerStopInteract_Implementation()
{
	_character->OnInteractButtonHeld.RemoveDynamic(this, &APW_ChargeableMechanism::OnChargeButtonHeld);
	_character = nullptr;
}

bool APW_ChargeableMechanism::HasServerInteraction_Implementation()
{
	return true;
}

bool APW_ChargeableMechanism::IsInteracting_Implementation()
{
	return _character != nullptr;
}

void APW_ChargeableMechanism::OnChargeButtonHeld(bool bValue)
{
	_chargeActivated = bValue;
}

bool APW_ChargeableMechanism::IsLanternEquipped(AActor* interactingActor)
{
	const APW_Character* character = Cast<APW_Character>(interactingActor);
	
	if (!character)
		return false;
	
	UPW_InventoryHandler* inventory = Cast< UPW_InventoryHandler>(character->GetComponentByClass(UPW_InventoryHandler::StaticClass()));
	if (!inventory)
		return false;

	_lantern  = Cast<APW_Lantern>(inventory->GetCurrentItem());
	return _lantern != nullptr;
}

void APW_ChargeableMechanism::HandleCharging(float DeltaTime)
{
	
}

