// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/ActorComponent.h"
#include "PW_WeaponHandlerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_WEST_API UPW_WeaponHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Handler", meta = (AllowPrivateAccess = "true"))
	class UPW_WeaponData* _defaultWeaponData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Handler", meta = (AllowPrivateAccess = "true"))
	class UPW_WeaponVisualData* _defaultWeaponVisualData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Handler", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class APW_Weapon> _defaultWeaponClass;

	UPROPERTY(EditAnywhere, Category = "Weapon Handler")
	TSubclassOf<APW_Weapon> _weaponBlueprint;

	UPROPERTY(EditAnywhere, Category = "Weapon Handler")
	float _maximumWeaponFallOffRange = 10000.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon Handler")
	float _lastFiredTime = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon Handler")
	class APW_Character* _ownerCharacter;

	UPROPERTY()
	class UPW_ItemHandlerComponent* _itemHandlerComponent;

	UPROPERTY(EditAnywhere, Category = "Weapon Handler")
	bool _canAutoFire = true;
	
	bool _isFiring = false;
	FTimerHandle _reloadTimerHandle;
	FTimerHandle _fireTimerHandle;

public:	
	UPW_WeaponHandlerComponent();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	APW_Weapon* TryGetCurrentWeapon();

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void CastBulletRays(const UPW_WeaponData* weaponData);
	void CastBulletRay(UCameraComponent* cameraComponent);
	bool CastRay(const FVector& rayStart, const FVector& rayDestination, const FCollisionQueryParams& collisionQueryParams, FHitResult& hitResult) const;
	void ApplyDamage(const FHitResult& hitResult);
	void LocalApplyDamage(const FHitResult& hitResult);
	float CalculateDamage(const FHitResult& hitResult);
	bool CalculateFireStatus();
	void AssignInputActions();
	void LocalReloadWeapon();
	void OnReloadWeaponComplete();
	void FireWeaponVisual();
	void GetOwnerCharacter();
	
	UFUNCTION() void BeginFireSequence();
	void CoreFireSequence(APW_Weapon* currentWeapon, UPW_WeaponData* weaponData);
	UFUNCTION() void CompleteFireSequence();
	void QueueAutomaticFire(APW_Weapon* currentWeapon, UPW_WeaponData* weaponData);
	UFUNCTION() void ReloadWeapon();
	UFUNCTION(Server, Reliable) void ServerReloadWeapon();
	UFUNCTION(Server, Reliable) void ServerApplyDamage(const FHitResult& hitResult);
	
	FORCEINLINE void SetOwnerCharacter(APW_Character* ownerCharacter) { _ownerCharacter = ownerCharacter; }
};

