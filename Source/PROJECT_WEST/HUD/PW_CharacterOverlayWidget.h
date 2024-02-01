// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PW_CharacterOverlayWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_WEST_API UPW_CharacterOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* _healthBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* _healthText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* _scoreText;

	UPROPERTY(meta = (BindWidget))
	class UImage* _highPingImage;

	UPROPERTY(meta = (BindWidgetAnim) ,Transient)
	class UWidgetAnimation* _highPingAnimation;

public:
	void SetHealth(float health, float maxHealth);
	void SetScore(float score);

	void StartHighPingWarning();
	void StopHighPingWarning();
	bool IsHighPingWarningPlaying();
};
