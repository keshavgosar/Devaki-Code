// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainOverlay.generated.h"

/**
 * 
 */
UCLASS()
class DEVAKIVILLAGE_API UMainOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
		void NativeConstruct() override;

	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetGold(int32 GoldAmount);
	void SetSouls(int32 SoulsAmount);

private:
	
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* HealthProgressBar;

	UPROPERTY(meta=(BindWidget))
	UProgressBar* StaminaProgressBar;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* GoldText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* SoulsText;
	
};
