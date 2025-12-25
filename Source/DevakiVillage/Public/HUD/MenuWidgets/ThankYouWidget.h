// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ThankYouWidget.generated.h"

/**
 * 
 */
UCLASS()
class DEVAKIVILLAGE_API UThankYouWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget))
	class UButton* QuitToMainMenuButton;

	UFUNCTION()
	void QuitClicked();
	
};
