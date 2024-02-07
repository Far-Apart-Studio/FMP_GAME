// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PW_HUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshairs")
	UTexture2D* _crosshairsCenter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshairs")
	UTexture2D* _crosshairsLeft;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshairs")
	UTexture2D* _crosshairsRight;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshairs")
	UTexture2D* _crosshairsTop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshairs")
	UTexture2D* _crosshairsBottom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshairs")
	float _crosshairSpread;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshairs")
	FLinearColor _crosshairsColor;
};

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API APW_HUD : public AHUD
{
	GENERATED_BODY()

private:

	UPROPERTY( EditAnywhere, Category = "Widgets", meta = (AllowPrivateAccess = "true") )
	TSubclassOf<class UUserWidget> _characterOverlayWidgetClass;

	UPROPERTY( EditAnywhere, Category = "Widgets", meta = (AllowPrivate = "true") )
	TSubclassOf<class UUserWidget> _announcementWidgetClass;

	UPROPERTY( VisibleAnywhere, Category = "Widgets", meta = (AllowPrivate= "true") )
	class UPW_CharacterOverlayWidget* _characterOverlayWidget;

	UPROPERTY( VisibleAnywhere, Category = "Widgets", meta = (AllowPrivate= "true") )
	class UPW_AnnouncementWidget*  _announcementWidget;

	FTimerHandle _announcementTimer;
	
public:
	
	virtual void DrawHUD() override;

	FORCEINLINE class UPW_CharacterOverlayWidget* GetCharacterOverlayWidget() const { return _characterOverlayWidget; }
	FORCEINLINE class UPW_AnnouncementWidget* GetAnnouncementWidget() const { return _announcementWidget; }


	void DisplayAccouncement(const FString& message,FColor color = FColor::White, float duration = 2.0f);
	void HideAccouncement();

protected:

	APW_HUD();
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	void AddCharacterOverlayWidget();

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	FHUDPackage hudPackage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UTexture2D* hitMarker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float crosshairSpreadMax;

	class AFPS_Character* character;

	FVector2D screenCentre;
	FVector2D crosshairCentre;
	float crosshairSpreadMultiplier;
	float crosshairHalfWidth;
	float crosshairHalfHeight;
	
	FTimerHandle hitMarkerTimer;
	float hitMarkerDuration;
	bool showHitMarker;

private:

	void HandleScreenPosition();
	void HandleCrosshairCentre();

	void DrawMiddleCrosshair();	
	void DrawTopCrosshair();
	void DrawBottomCrosshair();
	void DrawLeftCrosshair();
	void DrawRightCrosshair();

	void TriggerHitMarker();
	void DrawHitMarker();
	void ResetHitMarker();
};
