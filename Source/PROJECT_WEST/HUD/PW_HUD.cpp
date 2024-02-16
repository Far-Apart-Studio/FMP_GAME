// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_HUD.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "PW_CharacterOverlayWidget.h"
#include "PW_AnnouncementWidget.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "PROJECT_WEST/DebugMacros.h"
#include "UObject/ConstructorHelpers.h"

void APW_HUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw crosshair
	HandleScreenPosition();
	HandleCrosshairCentre();
	
	DrawMiddleCrosshair();
	DrawTopCrosshair();
	DrawBottomCrosshair();
	DrawLeftCrosshair();
	DrawRightCrosshair();
}

APW_HUD::APW_HUD()
{
	crosshairHalfWidth = 32.0f;
	crosshairHalfHeight = 32.0f;
	crosshairSpreadMax = 16.0f;
}

void APW_HUD::BeginPlay()
{
	Super::BeginPlay();
	crosshairSpreadMultiplier = 1;
}

void APW_HUD::Destroyed()
{
	HideAccouncement();
	Super::Destroyed();
}

void APW_HUD::AddCharacterOverlayWidget()
{
	APlayerController* playerController = GetOwningPlayerController();
	if (playerController != nullptr && _characterOverlayWidgetClass != nullptr)
	{
		_characterOverlayWidget = CreateWidget<UPW_CharacterOverlayWidget>(playerController, _characterOverlayWidgetClass);
		if (_characterOverlayWidget != nullptr)
		{
			_characterOverlayWidget->AddToViewport();
		}
	}
}

void APW_HUD::DisplayAccouncement(const FString& message, FColor color, float duration)
{
	APlayerController* playerController = GetOwningPlayerController();
	if (playerController != nullptr)
	{
		_announcementWidget = CreateWidget<UPW_AnnouncementWidget>(playerController, _announcementWidgetClass);
		if (_announcementWidget)
		{
			_announcementWidget->SetAnnouncementText(message,color);
			_announcementWidget->AddToViewport();
			GetWorldTimerManager().SetTimer(_announcementTimer, this, &APW_HUD::HideAccouncement, duration, false);
		}
	}
}

void APW_HUD::HideAccouncement()
{
	if (_announcementWidget != nullptr)
	{
		_announcementWidget->RemoveFromViewport();
	}
}

void APW_HUD::SetVisibility(bool bShow)
{
	if(_characterOverlayWidget != nullptr)
	{
		_characterOverlayWidget->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void APW_HUD::HandleScreenPosition()
{
	screenCentre = FVector2D(Canvas->ClipX / 2, Canvas->ClipY / 2);
}

void APW_HUD::HandleCrosshairCentre()
{
	crosshairCentre = FVector2D(screenCentre.X - crosshairHalfWidth, screenCentre.Y - crosshairHalfHeight);
}

void APW_HUD::DrawMiddleCrosshair()
{
	if(!hudPackage._crosshairsCenter) return;
	const FVector2D drawPosition(FVector2D(crosshairCentre.X , crosshairCentre.Y));
	DrawTexture(hudPackage._crosshairsCenter, drawPosition.X, drawPosition.Y, crosshairHalfWidth * 2, crosshairHalfHeight * 2, 0, 0, 1, 1);
}

void APW_HUD::DrawTopCrosshair()
{
	if(!hudPackage._crosshairsTop) return;
	const FVector2D drawPosition(FVector2D(crosshairCentre.X, crosshairCentre.Y - ( crosshairSpreadMax * crosshairSpreadMultiplier)));
	DrawTexture (hudPackage._crosshairsTop, drawPosition.X, drawPosition.Y, crosshairHalfWidth * 2, crosshairHalfHeight * 2, 0, 0, 1, 1);
}

void APW_HUD::DrawBottomCrosshair()
{
	if(!hudPackage._crosshairsBottom) return;
	const FVector2D drawPosition(FVector2D(crosshairCentre.X, crosshairCentre.Y + (crosshairSpreadMax * crosshairSpreadMultiplier)));
	DrawTexture(hudPackage._crosshairsBottom, drawPosition.X, drawPosition.Y, crosshairHalfWidth * 2, crosshairHalfHeight * 2, 0, 0, 1, 1);
}

void APW_HUD::DrawLeftCrosshair()
{
	if(!hudPackage._crosshairsLeft) return;
	const FVector2D drawPosition(FVector2D(crosshairCentre.X - (crosshairSpreadMax * crosshairSpreadMultiplier), crosshairCentre.Y));
	DrawTexture(hudPackage._crosshairsLeft, drawPosition.X, drawPosition.Y, crosshairHalfWidth * 2, crosshairHalfHeight * 2, 0, 0, 1, 1);
}

void APW_HUD::DrawRightCrosshair()
{
	if(!hudPackage._crosshairsRight) return;
	const FVector2D drawPosition(FVector2D(crosshairCentre.X + (crosshairSpreadMax * crosshairSpreadMultiplier), crosshairCentre.Y));
	DrawTexture(hudPackage._crosshairsRight, drawPosition.X, drawPosition.Y, crosshairHalfWidth * 2, crosshairHalfHeight * 2, 0, 0, 1, 1);
}

void APW_HUD::TriggerHitMarker()
{
}

void APW_HUD::DrawHitMarker()
{
}

void APW_HUD::ResetHitMarker()
{
}
