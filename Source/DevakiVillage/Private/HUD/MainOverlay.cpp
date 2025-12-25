// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MainOverlay.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "HUD/BossHealthBar.h"


void UMainOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void UMainOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(Percent);
	}
}

void UMainOverlay::SetGold(int32 GoldAmount)
{
	if (GoldText)
	{
		FText GoldTextValue = FText::FromString(FString::Printf(TEXT("%d"), GoldAmount));
		GoldText->SetText(GoldTextValue);
	}
}

void UMainOverlay::SetSouls(int32 SoulsAmount)
{
	if (SoulsText)
	{
		FText SoulsTextValue = FText::FromString(FString::Printf(TEXT("%d"), SoulsAmount));
		SoulsText->SetText(SoulsTextValue);
	}
}

void UMainOverlay::ShowBossHealthBar()
{
	if (BossHealthBar)
	{
		BossHealthBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMainOverlay::UpdateBossHealthBar(float Percent)
{
	if (BossHealthBar)
	{
		BossHealthBar->SetHealthBarPercent(Percent);
	}
}

void UMainOverlay::HideBossHealthBar()
{
	if (BossHealthBar)
	{
		BossHealthBar->SetVisibility(ESlateVisibility:: Hidden);
	}
}

void UMainOverlay::ShowInteractionPrompt(const FString& PromptText)
{
	if (InteractionPromptText)
	{
		InteractionPromptText->SetText(FText::FromString(PromptText));
		InteractionPromptText->SetVisibility(ESlateVisibility::Visible);
        
		UE_LOG(LogTemp, Warning, TEXT("Interaction Prompt Shown: %s"), *PromptText);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("InteractionPromptText is NULL!"));
	}
}

void UMainOverlay::HideInteractionPrompt()
{
	if (InteractionPromptText)
	{
		InteractionPromptText->SetVisibility(ESlateVisibility::Hidden);
	}
}