// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MainOverlay.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UMainOverlay::NativeConstruct()
{
	Super::NativeConstruct();
}

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
		GoldText->SetText(SoulsTextValue);
	}
}
