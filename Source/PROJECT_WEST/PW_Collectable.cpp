#include "PW_Collectable.h"

#include "DebugMacros.h"

APW_Collectable::APW_Collectable(): _meshComponent(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void APW_Collectable::BeginPlay()
{
	Super::BeginPlay();
	SetReplicates(true);
	SetReplicateMovement(true);
}

void APW_Collectable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APW_Collectable::StartFocus_Implementation(AActor* owner)
{
	IPW_InteractableInterface::StartFocus_Implementation(owner);
	_meshComponent->SetRenderCustomDepth(true);
}

void APW_Collectable::EndFocus_Implementation(AActor* owner)
{
	IPW_InteractableInterface::EndFocus_Implementation(owner);
	_meshComponent->SetRenderCustomDepth(false);
}

void APW_Collectable::StartInteract_Implementation(AActor* owner)
{
	IPW_InteractableInterface::StartInteract_Implementation(owner);
	DEBUG_STRING("APW_Collectable::StartInteract_Implementation");

	if (_meshComponent)
		_meshComponent->SetVisibility(false);

	OnCollected.Broadcast();
}

