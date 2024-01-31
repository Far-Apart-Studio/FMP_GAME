// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_HUD.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "PW_CharacterOverlayWidget.h"

void APW_HUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw crosshair
}

void APW_HUD::BeginPlay()
{
	Super::BeginPlay();
	AddCharacterOverlayWidget();
}

void APW_HUD::AddCharacterOverlayWidget()
{
	APlayerController* playerController = GetOwningPlayerController();
	if (playerController != nullptr)
	{
		_characterOverlayWidget = CreateWidget<UPW_CharacterOverlayWidget>(playerController, _characterOverlayWidgetClass);
		if (_characterOverlayWidget != nullptr)
		{
			_characterOverlayWidget->AddToViewport();
		}
	}
}
