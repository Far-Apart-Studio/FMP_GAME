// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PW_WeaponHandlerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_WeaponHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Handler")
	class UPW_WeaponData* _defaultWeaponData;

	UPROPERTY(EditAnywhere, Category = "Weapon Handler")
	class UPW_WeaponVisualData* _defaultWeaponVisualData;
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon Handler")
	class APW_Weapon* _currentWeapon;

	UPROPERTY(EditAnywhere, Category = "Weapon Handler")
	float _maximumWeaponFallOffRange = 10000.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon Handler")
	float _lastFiredTime = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon Handler")
	class APW_Character* _ownerCharacter;

public:	
	UPW_WeaponHandlerComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void CastBulletRay();
	bool CastRay(const FVector& rayStart, const FVector& rayDestination, const FCollisionQueryParams& collisionQueryParams, FHitResult& hitResult) const;
	void AttachDefaultWeapon();
	void ApplyDamage(const FHitResult& hitResult) const;
	float CalculateDamage(const FHitResult& hitResult) const;
	bool CalculateFireStatus();
	void GetOwnerCharacter();
	void AssignInputActions();

	UFUNCTION() void ReloadWeapon();
	UFUNCTION() void FireWeapon();
	void FireWeaponVisual();
	void ReloadWeaponVisual();
	FORCEINLINE APW_Weapon* GetCurrentWeapon() const { return _currentWeapon; }
	FORCEINLINE void SetCurrentWeapon(APW_Weapon* currentWeapon) { _currentWeapon = currentWeapon; }
};

