// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Souls.generated.h"

/**
 * 
 */
UCLASS()
class DEVAKIVILLAGE_API ASouls : public AItem
{
	GENERATED_BODY()

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere, Category="Souls Properties")
	int32 SoulsAmount;

public:
	FORCEINLINE int32 GetSoulsAmount() const { return SoulsAmount; }
	FORCEINLINE void SetSoul(int32 NewSoulsAmount) {SoulsAmount = NewSoulsAmount;};
};
