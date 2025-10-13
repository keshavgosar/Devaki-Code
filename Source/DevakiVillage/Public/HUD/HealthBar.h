// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class DEVAKIVILLAGE_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;
	
	
	
};
