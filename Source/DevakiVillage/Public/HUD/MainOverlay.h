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

	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetGold(int32 GoldAmount);
	void SetSouls(int32 SoulsAmount);

	// Boss Health Bar Functions
	void ShowBossHealthBar();
	void UpdateBossHealthBar(float Percent);
	void HideBossHealthBar();

	// Boss Health Bar Widget
	UPROPERTY(meta = (BindWidget))
	class UBossHealthBar* BossHealthBar;
	
	// Interaction Prompt Functions
     UFUNCTION(BlueprintCallable)
     void ShowInteractionPrompt(const FString& PromptText);
        
	UFUNCTION(BlueprintCallable)
	void HideInteractionPrompt();

	// Interaction Prompt Widget
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InteractionPromptText;


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
