// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/GameplayMainHUD.h"

#include "Blueprint/UserWidget.h"
#include "HUD/MainOverlay.h"

void AGameplayMainHUD::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
	{
		APlayerController* PC = World->GetFirstPlayerController();
		if ( PC && MainOverlayClass)
		{
			MainOverlay = CreateWidget<UMainOverlay>(PC, MainOverlayClass);
			MainOverlay->AddToViewport();
		}
	}
}
