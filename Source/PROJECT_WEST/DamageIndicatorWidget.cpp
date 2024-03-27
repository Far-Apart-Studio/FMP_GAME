// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageIndicatorWidget.h"

#include "DebugMacros.h"
#include "PWMath.h"
#include "PW_Utilities.h"
#include "Kismet/GameplayStatics.h"

void UDamageIndicatorWidget::SelfDestruct()
{
	FTimerHandle timerHandle;
	GetWorld()->GetTimerManager().SetTimer(timerHandle, this,
		&UDamageIndicatorWidget::RemoveFromParent, _indicateHandle.duration, false);
}

float UDamageIndicatorWidget::GetClampedAngle()
{
	float dotProduct = 0.0f;
	float crossProduct = 0.0f;

	GetDotAndCrossProductBetweenEnemy(dotProduct, crossProduct);
	
	const float angle = -FMath::Atan2(crossProduct, -dotProduct);
	const float angleDegrees = FMath::RadiansToDegrees(angle);
	const float clampedAngle = FMath::GetMappedRangeValueClamped
		(FVector2D(180.0f, -180.0f), FVector2D(0.0, 1.0), angleDegrees);

	return clampedAngle;
}

float UDamageIndicatorWidget::GetDotProductBetweenEnemy()
{
	const AActor* ownerActor = _indicateHandle.ownerActor;
	const AActor* damageCauser = _indicateHandle.damageCauser;

	if (ownerActor == nullptr)
	{ PW_Utilities::Log("OWNER ACTOR NULL"); return 0.0f;}

	if (damageCauser == nullptr)
	{ PW_Utilities::Log("DAMAGE CAUSER NULL"); return 0.0f; }
	
	const FVector ownerForward = ownerActor->GetActorForwardVector();
	const FVector ownerPosition = ownerActor->GetActorLocation();
	const FVector damageCauserPosition = damageCauser->GetActorLocation();
	const FVector direction = (damageCauserPosition - ownerPosition);
	const FVector directionNormalized = direction.GetSafeNormal();
	const float dotProduct = FVector::DotProduct(ownerForward, directionNormalized);

	return dotProduct;
}

float UDamageIndicatorWidget::GetCrossProductBetweenEnemy()
{
	const AActor* ownerActor = _indicateHandle.ownerActor;
	const AActor* damageCauser = _indicateHandle.damageCauser;

	if (ownerActor == nullptr)
		{ PW_Utilities::Log("OWNER ACTOR NULL"); return 0.0f;}

	if (damageCauser == nullptr)
		{ PW_Utilities::Log("DAMAGE CAUSER NULL"); return 0.0f; }

	const FVector ownerForward = ownerActor->GetActorForwardVector();
	const FVector ownerPosition = ownerActor->GetActorLocation();
	const FVector damageCauserPosition = damageCauser->GetActorLocation();
	const FVector direction = (damageCauserPosition - ownerPosition);
	const FVector directionNormalized = direction.GetSafeNormal();
	
	const float crossProduct = ownerForward.X * directionNormalized.Y - ownerForward.Y * directionNormalized.X;
	return crossProduct;
}

void UDamageIndicatorWidget::GetDotAndCrossProductBetweenEnemy(float& dotProduct, float& crossProduct)
{
	const AActor* ownerActor = _indicateHandle.ownerActor;
	const AActor* damageCauser = _indicateHandle.damageCauser;

	if (ownerActor == nullptr)
		{ PW_Utilities::Log("OWNER ACTOR NULL"); return; }

	if (damageCauser == nullptr)
		{ PW_Utilities::Log("DAMAGE CAUSER NULL"); return; }

	const FVector ownerForward = ownerActor->GetActorForwardVector();
	const FVector ownerPosition = ownerActor->GetActorLocation();
	const FVector damageCauserPosition = damageCauser->GetActorLocation();
	const FVector direction = (damageCauserPosition - ownerPosition);
	const FVector directionNormalized = direction.GetSafeNormal();
	
	dotProduct = FVector::DotProduct(ownerForward, directionNormalized);
	crossProduct = ownerForward.X * directionNormalized.Y - ownerForward.Y * directionNormalized.X;
}

void UDamageIndicatorWidget::ValidateRenderStatus()
{
	const float dotProduct = GetDotProductBetweenEnemy();
	SetRenderOpacity(dotProduct > _indicateHandle.renderThreshold ? 0.0f : 1.0f);
}





