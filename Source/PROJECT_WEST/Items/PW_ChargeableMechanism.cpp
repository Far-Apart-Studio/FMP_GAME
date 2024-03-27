#include "PW_ChargeableMechanism.h"

#include "PW_Lantern.h"
#include "Net/UnrealNetwork.h"
#include "PROJECT_WEST/PW_Character.h"
#include "PROJECT_WEST/PW_InventoryHandler.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "PROJECT_WEST/Character/Components/PW_InteractionComponent.h"
#include "PROJECT_WEST/Gameplay/Components/PW_HighlightCompont.h"
#include "PROJECT_WEST/PlayerController/PW_PlayerController.h"

APW_ChargeableMechanism::APW_ChargeableMechanism(): _character(nullptr), _lantern(nullptr), _chargeCompleted(false),
                                                    _highlightComponent(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	//bAlwaysRelevant = true;
}

void APW_ChargeableMechanism::BeginPlay()
{
	Super::BeginPlay();
	_highlightComponent = FindComponentByClass<UPW_HighlightCompont>();

	_OnChargeAmountChanged.Broadcast(_currentChargeAmount / _maxChargeAmount);
}

void APW_ChargeableMechanism::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_chargeActivated && !_chargeCompleted && HasAuthority())
	{
		HandleCharging(DeltaTime);
	}
}

void APW_ChargeableMechanism::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APW_ChargeableMechanism, _chargeCompleted);
	DOREPLIFETIME(APW_ChargeableMechanism, _currentChargeAmount);
	DOREPLIFETIME (APW_ChargeableMechanism, _chargeActivated);
}

void APW_ChargeableMechanism::ForceEndInteraction() const
{
	if (UPW_InteractionComponent* interactionComponent = Cast<UPW_InteractionComponent>(_character->GetComponentByClass(UPW_InteractionComponent::StaticClass())))
	{
		interactionComponent->TryEndInteractWithInteractable();
	}
}

void APW_ChargeableMechanism::OnRep_ChargeAmountChanged() const
{
	_OnChargeAmountChanged.Broadcast(_currentChargeAmount / _maxChargeAmount);
}

void APW_ChargeableMechanism::OnRep_ChargeStatusChanged() const
{
	if (_chargeCompleted)
	{
		_OnChargeCompleted.Broadcast();
	}
}

void APW_ChargeableMechanism::StartFocus_Implementation(AActor* targetActor)
{
	if (_chargeActivated) return;
	
	_OnFocusBegin.Broadcast(IsLanternEquipped(targetActor));
	_highlightComponent->ShowHighlight();
	
	//DEBUG_STRING (IsLanternEquipped(targetActor) ? "Lantern Equipped" : "No Lantern Equipped");
}

void APW_ChargeableMechanism::EndFocus_Implementation(AActor* owner)
{
	_OnFocusEnd.Broadcast();
	_highlightComponent->HideHighlight();
}

void APW_ChargeableMechanism::StartInteract_Implementation(AActor* targetActor)
{
	if(!IsLanternEquipped(targetActor)) return;
	
	_character = Cast<APW_Character>(targetActor);
	if (!_character)return;

	ToggleCharacterMovement(false);
	_character->OnInteractButtonToggled.AddDynamic(this, &APW_ChargeableMechanism::OnChargeButtonHeld);

	//DEBUG_STRING ("Start Local Interact");
}

void APW_ChargeableMechanism::EndInteract_Implementation()
{
	_character->OnInteractButtonToggled.RemoveDynamic(this, &APW_ChargeableMechanism::OnChargeButtonHeld);
	ToggleCharacterMovement(true);
	_character = nullptr;

	//DEBUG_STRING ("End Local Interacting");
}

bool APW_ChargeableMechanism::HasServerInteraction_Implementation()
{
	return true;
}

void APW_ChargeableMechanism::ServerStartInteract_Implementation(AActor* targetActor)
{
	if(!IsLanternEquipped(targetActor))
		return;
	
	_character = Cast<APW_Character>(targetActor);
	_chargeActivated  = true;
}

void APW_ChargeableMechanism::ServerStopInteract_Implementation()
{
	_chargeActivated = false;
	_character = nullptr;
}

bool APW_ChargeableMechanism::IsInteracting_Implementation()
{
	return _character != nullptr;
}

void APW_ChargeableMechanism::OnChargeButtonHeld(bool bValue)
{
	_chargeActivated = bValue;

	if (!bValue)
	{
		ForceEndInteraction();
	}

	//DEBUG_STRING (bValue ? "Charging" : "Not Charging");
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
	if(_lantern->IsFuelEmpty())
	{
		_chargeActivated = false;
		return;
	}
	
	_currentChargeAmount = FMath::Clamp(_currentChargeAmount + _chargeRate * DeltaTime, 0.0f, _maxChargeAmount);
	_lantern->LocalModifyFuel(- _chargeRate * DeltaTime);
	
	_OnChargeAmountChanged.Broadcast(_currentChargeAmount / _maxChargeAmount);

	if (IsFullyCharged())
	{
		_chargeCompleted = true;
		_OnChargeCompleted.Broadcast();
	}

	if(!IsLanternEquipped(_character))
	{
		_chargeActivated = false;
	}

	DEBUG_STRING ("Charging " + FString::SanitizeFloat(_currentChargeAmount));
}

void APW_ChargeableMechanism::ToggleCharacterMovement(bool bValue) const
{
	if (!_character)
		return;
	
	if (_character && _character->IsLocallyControlled())
	{
		_character->ToggleMovement(bValue);
	}
}

bool APW_ChargeableMechanism::IsFullyCharged() const
{
	return  _currentChargeAmount >= _maxChargeAmount;
}

