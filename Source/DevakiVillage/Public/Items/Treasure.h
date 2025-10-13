// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Treasure.generated.h"

/**
 * 
 */
UCLASS()
class DEVAKIVILLAGE_API ATreasure : public AItem
{
	GENERATED_BODY()

public:
	
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere, Category="Treasure Sound")
	USoundBase* TreasurePickSound;

	UPROPERTY(EditAnywhere, Category="Gold Amount")
	int32 GoldAmount;
};
