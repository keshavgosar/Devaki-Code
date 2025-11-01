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

public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere, Category="Souls Properties")
	int32 SoulsAmount;

	double DesiredZ;

	UPROPERTY(EditAnywhere, Category="Souls Properties")
	float DriftRate = -15.f;

public:
	FORCEINLINE int32 GetSoulsAmount() const { return SoulsAmount; }
	FORCEINLINE void SetSoul(int32 NewSoulsAmount) {SoulsAmount = NewSoulsAmount;};
};
