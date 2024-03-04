// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PROJECT_WEST/Gameplay/Components/PW_MaterialEffectComponent.h"
#include "PW_SnareTrap.generated.h"

UCLASS()
class PROJECT_WEST_API APW_SnareTrap : public AActor
{
	GENERATED_BODY()
	
public:	

	APW_SnareTrap();

protected:

	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnHealthChanged();

	UFUNCTION()
	void OnDeath(AActor* OwnerActor, AActor* DamageCauser, AController* DamageCauserController);

public:	

	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* _mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* _skeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* _triggerVolume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	class UAnimationAsset* _catchAnimation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	class UAnimationAsset* _releaseAnimation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	class UPW_ActorMoverComponent* _actorMoverComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	class UPW_HealthComponent* _healthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	class UPW_MaterialEffectComponent* _materialEffectComponent;
	
	UPROPERTY (VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* _point1;

	UPROPERTY (VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* _point2;

	UPROPERTY(ReplicatedUsing = OnRep_OnStatsChanged, EditAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	bool _isSnareActive;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	float _damageRate;

	class APW_Character* _caughtCharacter;


	UFUNCTION()
	void OnRep_OnStatsChanged();

	UFUNCTION()
	void OnHighlightComplete(EEffectDirection Direction);

	void CaughtCharacter( APW_Character* Character );
	void DrainHealthOfCaughtCharacter(float DeltaTime);

	void ToggleCollisionQuery(bool bEnable);
};
