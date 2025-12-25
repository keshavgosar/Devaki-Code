// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MenuWidgets/StartMenuWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UStartMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &UStartMenuWidget::OnStartClicked);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UStartMenuWidget::OnQuitClicked);
	}
}

void UStartMenuWidget::OnStartClicked()
{
	if (StartButton)
	{
		if (UWorld* World = GetWorld())
		{
			UGameplayStatics::OpenLevel(World, FName("MainLevel"));
		}
	}
}

void UStartMenuWidget::OnQuitClicked()
{
	if (QuitButton)
	{
		if (UWorld* World = GetWorld())
		{
			UKismetSystemLibrary::QuitGame(
				World,
				nullptr,
				EQuitPreference::Quit,
				true
			);
		}
	}
}
