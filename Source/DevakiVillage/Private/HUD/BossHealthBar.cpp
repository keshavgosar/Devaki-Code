// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/BossHealthBar.h"

#include "Components/ProgressBar.h"

void UBossHealthBar::SetHealthBarPercent(float Percent)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(Percent);
	}
}