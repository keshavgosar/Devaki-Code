// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"

#include "NavigationPath.h"
#include "Component/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Runtime/AIModule/Classes/AIController.h"


AEnemy::AEnemy()
{
 	
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	HealthBarWidgetClass = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidgetClass->SetupAttachment(GetRootComponent());

	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	PawnSensingComponent->SightRadius = 4000.0f;
	PawnSensingComponent->SetPeripheralVisionAngle(45.f);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (PawnSensingComponent)
	{
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemy::OnPawnSeen);
	}

	if (HealthBarWidgetClass)
	{
		HealthBarWidgetClass->SetVisibility(false);
	}

	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
}


void AEnemy::CheckCombatTarget()
{
	if (!InTargetRange(CombatTarget, CombatRadius))
	{
		//Loose Interest when the target is out of range
		CombatTarget = nullptr;
		if (HealthBarWidgetClass)
		{
			HealthBarWidgetClass->SetVisibility(false);
		}
		EnemyState = EEnemyState::EES_Patrolling;
		GetCharacterMovement()->MaxWalkSpeed = 150.f;
		MoveToTarget(PatrolTarget);
	}
	else if (!InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Chasing)
	{
		//Outside the attack range chase character
		EnemyState = EEnemyState::EES_Chasing;
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		MoveToTarget(CombatTarget);
	}
	else if (InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Attacking)
	{
		//Inside the attack range attack character
		EnemyState =  EEnemyState::EES_Attacking;
	}
	
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(WaitTimeMin, WaitTimeMax);
		GetWorldTimerManager().SetTimer(PatrolTimerHandle, this, &AEnemy::PatrolTimerFinished, WaitTime);
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}

}


void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::Die()
{
	if (HealthBarWidgetClass)
	{
		HealthBarWidgetClass->SetVisibility(false);
	}
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);

		int32 DeathVal = FMath::RandRange(0, 4);
		FName SectionName = FName();

		switch (DeathVal)
		{
			
		case 0:
			SectionName = "Death1";
			DeathPose = EDeathPose::EDP_Death1;
			break;

		case 1:
			SectionName = "Death2";
			DeathPose = EDeathPose::EDP_Death2;
			break;

		case 2:
			SectionName = "Death3";
			DeathPose = EDeathPose::EDP_Death3;
			break;

		case 3:
			SectionName = "Death4";
			DeathPose = EDeathPose::EDP_Death4;
			break;

		case 4:
			SectionName = "Death5";
			DeathPose = EDeathPose::EDP_Death5;
			break;

		default:
			break;
			
		}
		
		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(1.f);
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	
	//Get Distance to Target
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(15.f);
	EnemyController->MoveTo(MoveRequest);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	
	TArray<AActor*> ValidTargets;
	for (AActor* ValidTarget : PatrolTargets)
	{
		if (ValidTarget != PatrolTarget)
		{
			ValidTargets.Add(ValidTarget);
		}
	}

	if (PatrolTargets.Num() > 0)
	{
		AActor* Target = ValidTargets[FMath::RandRange(0, PatrolTargets.Num() - 1)];
		PatrolTarget = Target;
		return Target;
	}
	
	return nullptr;
	
}

void AEnemy::OnPawnSeen(APawn* SeenPawn)
{
	if (EnemyState == EEnemyState::EES_Chasing) return;
	
	if (SeenPawn->ActorHasTag(FName("AaravCharacter")))
	{
		GetWorldTimerManager().ClearTimer(PatrolTimerHandle);
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		CombatTarget = SeenPawn;

		if (EnemyState != EEnemyState::EES_Attacking)
		{
			EnemyState = EEnemyState::EES_Chasing;
			MoveToTarget(CombatTarget);
		}
	}
}


void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	if (HealthBarWidgetClass)
	{
		HealthBarWidgetClass->SetVisibility(true);
	}
	
	if (AttributeComponent && AttributeComponent->IsAlive())
	{
		DirectionalHitReact(ImpactPoint);
	}
	else
	{
		Die();
	}

	if (HitSound && HitParticle)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
		UGameplayStatics::SpawnEmitterAtLocation(this, HitParticle, ImpactPoint);
	}
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator,
	AActor* DamageCauser)
{
	if (AttributeComponent && HealthBarWidgetClass)
	{
		AttributeComponent->ReceiveDamage(DamageAmount);
		HealthBarWidgetClass->SetProgressBarPercent(AttributeComponent->GetHealthPercentage());
	}

	CombatTarget = EventInstigator->GetPawn();

	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	MoveToTarget(CombatTarget);
	return DamageAmount;
}

