// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterTypes.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"

class UAnimMontage;
class UAttributeComponent;
class UHealthBarComponent;
class AAIController;

UCLASS()
class DEVAKIVILLAGE_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	
	AEnemy();

	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void DirectionalHitReact(const FVector& ImpactPoint);

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

private:

	/*
	 * Animation Montages
	 */
	
	UPROPERTY(EditAnywhere, Category = "Anim Montages | Attack Montage")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = "Anim Montages | Death Montage")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category=Sounds)
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category="Particle Effects")
	UParticleSystem* HitParticle;

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidgetClass;

	UPROPERTY()
	AActor* CombatTarget;

	/*
	 * Navigation
	 */

	UPROPERTY()
	AAIController* EnemyController;

	//Current Target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigations")
	AActor* PatrolTarget;
	
	UPROPERTY(EditAnywhere, Category = "AI Navigations")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;

protected:

	void Die();

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;
	
	/*
	 * Play Montage Functions
	 */

	void PlayHitReactMontage(const FName& SectionName);
	
	virtual void BeginPlay() override;
	
	

};
