// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class DEVAKIVILLAGE_API UStartMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* StartButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;

	UPROPERTY(EditAnywhere, Category = "Level Gate")
	float FadeOutDuration = 2.f;

	UFUNCTION(BlueprintCallable)
	void OnStartClicked();

	UFUNCTION(BlueprintCallable)
	void OnQuitClicked();
	
};
