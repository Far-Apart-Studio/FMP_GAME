// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PW_HUD.generated.h"

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
