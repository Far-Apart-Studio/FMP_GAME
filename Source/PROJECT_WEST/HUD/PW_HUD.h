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
	class UTexture2D* _crosshairsCenter;
	UTexture2D* _crosshairsLeft;
	UTexture2D* _crosshairsRight;
	UTexture2D* CrosshairsTop;
	UTexture2D* _crosshairsBottom;
	float _crosshairSpread;
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
	
	class UPW_CharacterOverlayWidget*_characterOverlayWidget;
	
public:
	
	virtual void DrawHUD() override;

	FORCEINLINE class UPW_CharacterOverlayWidget* GetCharacterOverlayWidget() const { return _characterOverlayWidget; }

protected:
	virtual void BeginPlay() override;
	void AddCharacterOverlayWidget();
};
