// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Character/CharacterTypes.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;

UCLASS()
class DEVAKIVILLAGE_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

	

protected:

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* HitActor) override;

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	virtual void DodgeEnd();

	virtual void BeginPlay() override;

	virtual void Attack();

	UFUNCTION(BlueprintNativeEvent)
	void Die();
	
	void DirectionalHitReact(const FVector& ImpactPoint);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticle(const FVector& ImpactPoint);
	virtual bool CanAttack();
	bool IsAlive();
	virtual void HandleDamage(float DamageAmount);
	void DisableCapsule();
	void DisableMeshCollision();

	/*
	 * Play Montage Functions
	 */
	
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	virtual void PlayDodgeMontage();
	void PlayHitReactMontage(const FName& SectionName);
	void StopAttackMontage();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWeapon* EquippedWeapon;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double WarpTargetDistance = 45.f;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

	/*
	 * Components
	 */
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* AttributeComponent;

private:

	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);
	/*
	 * Sound and Effects
	 */
	UPROPERTY(EditAnywhere, Category=Sounds)
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category="Particle Effects")
	UParticleSystem* HitParticle;

	/*
	 * Play Montages Variables
	 */
	UPROPERTY(EditAnywhere, Category = "Anim Montages | Attack Montage")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category = "Anim Montages | Attack Montage")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = "Anim Montages | Death Montage")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Anim Montages | Death Montage")
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> AttackMontageSections;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> DeathMontageSections;

public:
	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() { return DeathPose; }
};
