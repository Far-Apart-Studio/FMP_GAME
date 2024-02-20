// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_CharacterOverlayWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Animation/WidgetAnimation.h"
#include "PROJECT_WEST/DebugMacros.h"

void UPW_CharacterOverlayWidget::NativeConstruct()
{
	Super::NativeConstruct();
	_highPingImage = Cast<UImage>(GetWidgetFromName(TEXT("_highPingImage")));
	_highPingAnimation = Cast<UWidgetAnimation>(GetWidgetFromName(TEXT("_highPingAnimation")));
}

void UPW_CharacterOverlayWidget::StartHighPingWarning()
{
	if (_highPingImage && _highPingAnimation)
	{
		_highPingImage->SetVisibility(ESlateVisibility::Visible);
		PlayAnimation(_highPingAnimation, 0,5);
	}
}

void UPW_CharacterOverlayWidget::StopHighPingWarning()
{
	if (_highPingImage)
	{
		_highPingImage->SetVisibility(ESlateVisibility::Hidden);
		if (IsAnimationPlaying( _highPingAnimation))
		{
			StopAnimation(_highPingAnimation);
		}
	}
}

bool UPW_CharacterOverlayWidget::IsHighPingWarningPlaying()
{
	if (_highPingImage)
	{
		return IsAnimationPlaying(_highPingAnimation);
	}
	return false;
}