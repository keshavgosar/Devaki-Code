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

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* HitActor) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	bool ActorIsSameType(AActor* OtherActor);

	UFUNCTION(BlueprintCallable)
	void EnemyWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);
	
	bool IsDead();

protected:
	/*
	 * Weapon Properties
	 */

	UPROPERTY(VisibleAnywhere, Category="Weapon Properties")
	USceneComponent* BoxTraceStart1;

	UPROPERTY(VisibleAnywhere, Category="Weapon Properties")
	USceneComponent* BoxTraceEnd1;

	UPROPERTY(VisibleAnywhere, Category="Weapon Properties")
	USceneComponent* BoxTraceStart2;

	UPROPERTY(VisibleAnywhere, Category="Weapon Properties")
	USceneComponent* BoxTraceEnd2;

	UPROPERTY(VisibleAnywhere, Category="Weapon Properties")
	class UBoxComponent* WeaponBox1;

	UPROPERTY(VisibleAnywhere, Category="Weapon Properties")
	UBoxComponent* WeaponBox2;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void SpawnSouls();

	//Enemy State
	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	virtual void Die_Implementation() override;
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();

	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);

	UPROPERTY(EditAnywhere, Category = "Combat")
	float DeathLifeSpan = 5.f;
	
	/*
	 * Play Montage Functions
	 */
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void AttackEnd() override;
	
	virtual void BeginPlay() override;
	

	virtual void HandleDamage(float DamageAmount) override;


private:
	void InitializeEnemy();
	/*
	 * AI Behaviour Functions
	 */
	void CheckCombatTarget();
	void CheckPatrolTarget();
	void PatrolTimerFinished();
	void ShowHealthBar();
	void HideHealthBar();
	void LooseInterest();
	void StartPatrolling();
	void ChaseTarget();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsChasing();
	bool IsInsideAttackRadius();
	bool IsAttacking();
	bool IsEngaged();

	void ClearPatrolTimer();

	/*
	 * Combat
	 */
	void StartAttackTimer();
	void ClearAttackTimer();

	/*
	 * Components
	 */

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidgetClass;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY(EditAnywhere, Category = Combat)
	double CombatRadius = 500.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	double AttackRadius = 150.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	double AcceptanceRadius = 50.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float DamageBox1 = 10.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float DamageBox2 = 10.f;

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

	UPROPERTY(EditAnywhere, Category = Combat)
	float PatrollingSpeed = 150.f;

	FTimerHandle PatrolTimerHandle;
	
	UPROPERTY(EditAnywhere, Category= "AI Navigations || Wait Time")
	float WaitTimeMin = 5.f;
	
	UPROPERTY(EditAnywhere, Category= "AI Navigations || Wait Time")
	float WaitTimeMax = 10.f;

	FTimerHandle AttackTimerHandle;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMinTime = 0.5f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMaxTime = 1.5f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ChasingSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Souls Class")
	TSubclassOf<class ASouls> SoulsClass;
	
	FTimerHandle SpawnTimerHandle;

};
