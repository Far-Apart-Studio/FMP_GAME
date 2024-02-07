// Fill out your copyright notice in the Description page of Project Settings.


#include "PW_AnnouncementWidget.h"
#include "Components/TextBlock.h"

void UPW_AnnouncementWidget::SetAnnouncementText(const FString& String, FColor Color)
{
	_announcementText->SetText(FText::FromString(String));
	_announcementText->SetColorAndOpacity(Color);
}
