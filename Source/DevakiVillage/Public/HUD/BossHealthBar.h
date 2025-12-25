// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossHealthBar.generated.h"

/**
 * 
 */
UCLASS()
class DEVAKIVILLAGE_API UBossHealthBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	void SetHealthBarPercent(float Percent);
};
