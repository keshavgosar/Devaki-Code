// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Public/Items/Item.h"
#include "Weapon.generated.h"

class USoundBase;
class UBoxComponent;
class USceneComponent;
/**
 * 
 */
UCLASS()
class DEVAKIVILLAGE_API AWeapon : public AItem
{
	GENERATED_BODY()

public:

	AWeapon();
	
	void AttachToSocket(USceneComponent* InParent, const FName& InSocketName);
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	void ExecuteGetHit(FHitResult BoxHit);

	virtual void BeginPlay() override;
	void PlayEquipSound();
	void DisableSphereCollision();
	void DeactivateEmbersParticle();

	UPROPERTY()
	TArray<AActor*> IgnoreActors;


protected:
	
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);
private:

	void BoxTrace(FHitResult& BoxHit);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector BoxTraceExtent = FVector(5.f);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowBoxDebug = false;

	UPROPERTY(VisibleAnywhere, Category="Weapon Properties")
	UBoxComponent* WeaponBox;

	UPROPERTY(VisibleAnywhere, Category="Weapon Properties")
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere, Category="Weapon Properties")
	USceneComponent* BoxTraceEnd;
	
	UPROPERTY(EditAnywhere, Category = "Sounds | Weapon Equip Sound")
	USoundBase* EquipSound;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage = 20.f;

public:

	FORCEINLINE UBoxComponent* GetBoxComponent() const { return WeaponBox; }
};
