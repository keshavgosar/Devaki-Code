// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/GameOverWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &UGameOverWidget::OnRestartButtonClicked);
	}
}

void UGameOverWidget::OnRestartButtonClicked()
{
	FString LevelName = GetWorld()->GetMapName();
	UGameplayStatics::OpenLevel(this, FName(*LevelName));
}
