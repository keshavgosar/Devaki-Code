// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameplayMainHUD.generated.h"

/**
 * 
 */
UCLASS()
class DEVAKIVILLAGE_API AGameplayMainHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<class UMainOverlay> MainOverlayClass;

	UPROPERTY()
	UMainOverlay* MainOverlay;

public:
	UMainOverlay* GetMainOverlay() const { return MainOverlay; }
	
};
