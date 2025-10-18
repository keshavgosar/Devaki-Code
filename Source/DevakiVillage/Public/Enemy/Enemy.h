// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Character/BaseCharacter.h"
#include "Character/CharacterTypes.h"
#include "Enemy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSeePawnDelegate, APawn*, Pawn);

class UAnimMontage;
class UHealthBarComponent;
class AAIController;
class UPawnSensingComponent;

UCLASS()
class DEVAKIVILLAGE_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	
	AEnemy();

	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

private:

	UPROPERTY()
	AActor* CombatTarget;

	/*
	 * Components
	 */

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidgetClass;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensingComponent;

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
	double PatrolRadius = 200.f;

	FTimerHandle PatrolTimerHandle;
	void PatrolTimerFinished();

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

	UPROPERTY(EditAnywhere, Category= "AI Navigations || Wait Time")
	float WaitTimeMin = 5.f;
	
	UPROPERTY(EditAnywhere, Category= "AI Navigations || Wait Time")
	float WaitTimeMax = 10.f;

	//Enemy State
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

protected:

	virtual void Die() override;
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();

	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;
	
	/*
	 * Play Montage Functions
	 */

	
	
	virtual void BeginPlay() override;
	void CheckCombatTarget();
	void CheckPatrolTarget();
};
