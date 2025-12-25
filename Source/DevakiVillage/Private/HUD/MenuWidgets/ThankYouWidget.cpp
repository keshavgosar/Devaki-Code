// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MenuWidgets/ThankYouWidget.h"

#include "Components/Button.h"
#include "HUD/MenuWidgets/StartMenuWidget.h"
#include "Kismet/GameplayStatics.h"

void UThankYouWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (QuitToMainMenuButton)
	{
		QuitToMainMenuButton->OnClicked.AddDynamic(this, &UThankYouWidget::QuitClicked);
	}
}

void UThankYouWidget::QuitClicked()
{
	if (QuitToMainMenuButton)
	{
		if (UWorld* World = GetWorld())
		{
			UGameplayStatics::OpenLevel(World, FName("MenuLevel"));
		}
	}
}
