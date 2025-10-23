// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"

#include "Component/AttributeComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Kismet/GameplayStatics.h"
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

	/*
	 * Weapon Trace Construction
	 */
	WeaponBox1 = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox1"));
	WeaponBox1->SetupAttachment(GetMesh(), FName("weapon_sword_r"));
	WeaponBox1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox1->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox1->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Ignore);

	WeaponBox2 = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox2"));
	WeaponBox2->SetupAttachment(GetMesh(), FName("weapon_sword_l"));
	WeaponBox2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox2->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox2->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Ignore);

	BoxTraceStart1 = CreateDefaultSubobject<USceneComponent>(TEXT("BoxTraceStartRight"));
	BoxTraceStart1->SetupAttachment(GetMesh(), FName("FX_Trail_02_R"));

	BoxTraceEnd1 = CreateDefaultSubobject<USceneComponent>(TEXT("BoxTraceEndRight"));
	BoxTraceEnd1->SetupAttachment(GetMesh(), FName("FX_Trail_01_R"));

	BoxTraceStart2 = CreateDefaultSubobject<USceneComponent>(TEXT("BoxTraceStartLeft"));
	BoxTraceStart2->SetupAttachment(GetMesh(), FName("FX_Trail_02_L"));

	BoxTraceEnd2 = CreateDefaultSubobject<USceneComponent>(TEXT("BoxTraceEndLeft"));
	BoxTraceEnd2->SetupAttachment(GetMesh(), FName("FX_Trail_01_L"));
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add("Enemy");
	
	if (PawnSensingComponent)
	{
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemy::OnPawnSeen);
	}

	if (WeaponBox1)
	{
		WeaponBox1->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnBoxOverlap);
	}

	if (WeaponBox2)
	{
		WeaponBox2->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnBoxOverlap);
	}

	InitializeEnemy();
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead()) return;
	
	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}

}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* HitActor)
{
	Super::GetHit_Implementation(ImpactPoint, HitActor);
	if (!IsDead()) ShowHealthBar();
	ClearPatrolTimer();
	ClearAttackTimer();
	
	EnemyWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

	StopAttackMontage();
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator,
	AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();

	if (IsInsideAttackRadius())
	{
		EnemyState = EEnemyState::EES_Attacking;
	}
	else if (IsOutsideAttackRadius())
	{
		ChaseTarget();
	}
	
	return DamageAmount;
}

bool AEnemy::ActorIsSameType(AActor* OtherActor)
{
	return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));
}

void AEnemy::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                          int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ActorIsSameType(OtherActor)) return;
	
	FVector StartLocationLeft = BoxTraceStart2->GetComponentLocation();
	FVector EndLocationLeft = BoxTraceEnd2->GetComponentLocation();

	FVector StartLocationRight = BoxTraceStart1->GetComponentLocation();
	FVector EndLocationRight = BoxTraceEnd1->GetComponentLocation();

	FHitResult BoxHitResultLeft;
	FHitResult BoxHitResultRight;

	UKismetSystemLibrary::BoxTraceSingle(
		this,
		StartLocationLeft,
		EndLocationLeft,
		FVector(5.f, 5.f, 5.f),
		BoxTraceStart2->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		TArray<AActor*> {this},
		EDrawDebugTrace::ForDuration,
		BoxHitResultLeft,
		true
		);

	UKismetSystemLibrary::BoxTraceSingle(
		this,
		StartLocationRight,
		EndLocationRight,
		FVector(5.f, 5.f, 5.f),
		BoxTraceStart1->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		TArray<AActor*> {this},
		EDrawDebugTrace::ForDuration,
		BoxHitResultRight,
		true
		);

	if (BoxHitResultLeft.GetActor())
	{
		// Applies the Damage
		if (ActorIsSameType(BoxHitResultLeft.GetActor())) return;
		
		UGameplayStatics::ApplyDamage(
			BoxHitResultLeft.GetActor(),
			10.f,
			GetInstigator()->GetController(),
			this,
			UDamageType::StaticClass()
			);
		
		IHitInterface* HitInterface = Cast<IHitInterface>(BoxHitResultLeft.GetActor());
		if (HitInterface)
		{
			HitInterface->Execute_GetHit(BoxHitResultLeft.GetActor(), BoxHitResultLeft.ImpactPoint, GetOwner());
		}
		
		//CreateFields(BoxHit.ImpactPoint);
	}

	if (BoxHitResultRight.GetActor())
	{
		if (ActorIsSameType(BoxHitResultRight.GetActor())) return;
		// Applies the Damage
		UGameplayStatics::ApplyDamage(
			BoxHitResultRight.GetActor(),
			10.f,
			GetInstigator()->GetController(),
			this,
			UDamageType::StaticClass()
			);
		
		IHitInterface* HitInterface = Cast<IHitInterface>(BoxHitResultRight.GetActor());
		if (HitInterface)
		{
			HitInterface->Execute_GetHit(BoxHitResultRight.GetActor(), BoxHitResultRight.ImpactPoint, GetOwner());
		}
		
		//CreateFields(BoxHit.ImpactPoint);
	}
}

void AEnemy::Die()
{
	EnemyState = EEnemyState::EES_Dead;
	PlayDeathMontage();
	ClearAttackTimer();
	HideHealthBar();
	DisableCapsule();
	SetLifeSpan(DeathLifeSpan);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
	MoveRequest.SetAcceptanceRadius(45.f);
	EnemyController->MoveTo(MoveRequest);
}

void AEnemy::OnPawnSeen(APawn* SeenPawn)
{
	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Dead &&
			EnemyState != EEnemyState::EES_Chasing &&
				EnemyState < EEnemyState::EES_Attacking &&
					SeenPawn->ActorHasTag(FName("AaravCharacter"));

	if (bShouldChaseTarget)
	{
		CombatTarget = SeenPawn;
		ClearPatrolTimer();
		ChaseTarget();
	}
	
}

void AEnemy::Attack()
{
	EnemyState = EEnemyState::EES_Engaged;
	Super::Attack();
	PlayAttackMontage();
	UE_LOG(LogTemp,Warning, TEXT("Enemy Attack Called"));
}

int32 AEnemy::PlayDeathMontage()
{
	const int32 Selection = Super::PlayDeathMontage();
	TEnumAsByte<EDeathPose> Pose(Selection);

	if (Pose < EDeathPose::EDP_Max)
	{
		DeathPose = Pose;
	}

	return Selection;
}

bool AEnemy::CanAttack()
{
	Super::CanAttack();
	bool bCanAttack =
		IsInsideAttackRadius() &&
		!IsAttacking() &&
		!IsEngaged() &&
		!IsDead();
	return bCanAttack;
}

void AEnemy::AttackEnd()
{
	Super::AttackEnd();

	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (AttributeComponent && HealthBarWidgetClass)
	{
		HealthBarWidgetClass->SetProgressBarPercent(AttributeComponent->GetHealthPercentage());
	}
}

void AEnemy::EnemyWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (WeaponBox1)
	{
		WeaponBox1->SetCollisionEnabled(CollisionEnabled);
	}

	if (WeaponBox2)
	{
		WeaponBox2->SetCollisionEnabled(CollisionEnabled);
	}
}

void AEnemy::InitializeEnemy()
{
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
	HideHealthBar();
}

void AEnemy::CheckCombatTarget()
{
	if (IsOutsideCombatRadius())
	{
		ClearAttackTimer();
		LooseInterest();
		
		if (!IsEngaged())
		{
			StartPatrolling();
		}
	}
	else if ( IsOutsideAttackRadius() && !IsChasing())
	{
		ClearAttackTimer();
		if (!IsEngaged())
		{
			ChaseTarget();
		}
	}
	else if ( CanAttack() )
	{
		StartAttackTimer();
		UE_LOG(LogTemp, Warning, TEXT("Attacking"));
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

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::ShowHealthBar()
{
	if (HealthBarWidgetClass)
	{
		HealthBarWidgetClass->SetVisibility(true);
	}
}

void AEnemy::HideHealthBar()
{
	if (HealthBarWidgetClass)
	{
		HealthBarWidgetClass->SetVisibility(false);
	}
}

void AEnemy::LooseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

void AEnemy::ChaseTarget()
{
	//Outside the attack range chase character
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(CombatTarget);
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimerHandle);
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackMinTime, AttackMaxTime);
	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimerHandle);
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

	if (ValidTargets.Num() > 0)
	{
		int32 Index = FMath::RandRange(0, ValidTargets.Num() - 1);
		AActor* Target = ValidTargets[Index];
		PatrolTarget = Target;
		return Target;
	}
	
	return nullptr;
	
}










