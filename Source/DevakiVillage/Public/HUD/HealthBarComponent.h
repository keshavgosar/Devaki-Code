// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"

class UHealthBar;
/**
 * 
 */
UCLASS()
class DEVAKIVILLAGE_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:

	void SetProgressBarPercent(float Percent);
	
	UPROPERTY()
	UHealthBar* HealthBarWidget;
};
