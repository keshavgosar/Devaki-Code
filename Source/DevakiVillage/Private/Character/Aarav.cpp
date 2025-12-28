// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Character/Aarav.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "Component/AttributeComponent.h"
#include "Enemy/Enemy.h"
#include "HUD/GameOverWidget.h"
#include "HUD/GameplayMainHUD.h"
#include "HUD/MainOverlay.h"
#include "Items/Souls.h"
#include "Items/Treasure.h"
#include "Items/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Public/Interfaces/InteractableInterface.h"

// Sets default values
AAarav::AAarav()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(FName("CameraComponent"));
	Camera->SetupAttachment(CameraBoom);
	Camera->bUsePawnControlRotation = false;

}



// Called when the game starts or when spawned
void AAarav::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("AaravCharacter"));

	if (GetMesh())
	{
		GetMesh()->HideBoneByName(TEXT("sword_bottom"), EPhysBodyOp::PBO_None);
		GetMesh()->HideBoneByName(TEXT("sword_top"), EPhysBodyOp::PBO_None);
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(AaravMappingContext, 0);
		}
	}

	InitializeMainOverlayWidget();
	
}

// Called every frame
void AAarav::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PerformInteractionTrace();

	if (AttributeComponent && MainOverlay)
	{
		AttributeComponent->RegenStamina(DeltaTime);
		MainOverlay->SetStaminaBarPercent(AttributeComponent->GetStaminaPercentage());
	}

}

// Called to bind functionality to input
void AAarav::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAarav::Move);
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAarav::Look);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &AAarav::Jump);
		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Started, this, &AAarav::SprintStart);
		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &AAarav::SprintCompleted);
		EnhancedInput->BindAction(InteractionAction, ETriggerEvent::Triggered, this, &AAarav::Interact);
		EnhancedInput->BindAction(AttackAction, ETriggerEvent::Triggered, this , &AAarav::Attack);
		EnhancedInput->BindAction(DodgeAction, ETriggerEvent::Triggered, this , &AAarav::Dodge);
	}

}

void AAarav::Jump()
{
	if (ActionState == EActionState::EAS_Unoccupied)
	{
		Super::Jump();
	}
}

void AAarav::GetHit_Implementation(const FVector& ImpactPoint, AActor* HitActor)
{
	Super::GetHit_Implementation(ImpactPoint, HitActor);

	TArray<AActor*> EnemyActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), EnemyActors);
	
	for (AActor* Actor : EnemyActors)
	{
		AEnemy* TempEnemy = Cast<AEnemy>(Actor);
		if (TempEnemy)
		{
			TempEnemy->EnemyWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
	
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

	if (AttributeComponent && AttributeComponent->GetHealthPercentage() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}
	
}

float AAarav::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator,
                         AActor* DamageCauser)
{
	HandleDamage(DamageAmount);

	SetHUDHealth();
	return DamageAmount;
}

void AAarav::SetOverlappingItem(class AItem* Item)
{
	OverlappedItem = Item;
}

void AAarav::AddSouls(class ASouls* Soul)
{
	if (AttributeComponent && Soul && MainOverlay)
	{
		AttributeComponent->AddSouls(Soul->GetSoulsAmount());
		MainOverlay->SetSouls(AttributeComponent->GetSouls());
	}
}

void AAarav::AddGold(class ATreasure* Treasure)
{
	if (AttributeComponent && Treasure && MainOverlay)
	{
		AttributeComponent->AddGold(Treasure->GetGoldAmount());
		MainOverlay->SetGold(AttributeComponent->GetGold());
	}
}


void AAarav::Move(const FInputActionValue& Value)
{
	/*if (ActionState != EActionState::EAS_Unoccupied) return;
	
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller && MovementVector != FVector2D::ZeroVector)
	{
		FRotator YawRotation = FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);

		FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection, MovementVector.Y);

		FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}*/

	// Allow movement even when attacking (for better combat feel)
	if (ActionState == EActionState::EAS_Dodge || ActionState == EActionState::EAS_Dead) return;
    
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller && MovementVector != FVector2D::ZeroVector)
	{
		FRotator YawRotation = FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);

		FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection, MovementVector.Y);

		FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AAarav::Look(const FInputActionValue& Value)
{
	
	FVector2D LookAxis = Value.Get<FVector2D>();

	if (Controller && LookAxis != FVector2D::ZeroVector) {

		AddControllerYawInput(LookAxis.X);
		AddControllerPitchInput(LookAxis.Y);

	}
}

void AAarav::SprintStart(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AAarav::SprintCompleted(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AAarav::Attack(const FInputActionValue& Value)
{
	
	if (CanAttack())
	{
		// Find nearest enemy
		FindNearestEnemyForAttack();
        
		// Play attack montage
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void AAarav::Dodge(const FInputActionValue& Value)
{
	if (IsOccupied() || !HasEnoughStamina()) return;
	
	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodge;

	if (AttributeComponent && MainOverlay)
	{
		AttributeComponent->UseStamina(AttributeComponent->GetDodgeCost());
		MainOverlay->SetStaminaBarPercent(AttributeComponent->GetStaminaPercentage());
	}
	
}

void AAarav::PerformInteractionTrace()
{
	// If FocusedActor is manually set (by gate trigger), keep it
	if (FocusedActor && FocusedActor->Implements<UInteractableInterface>())
	{
		// Don't override - it's set by something else (like gate trigger)
		return;
	}

	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + Camera->GetForwardVector() * TraceDistance;

	FHitResult OnHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(OnHit, Start, End, ECC_Visibility, Params);

	AActor* HitActor = OnHit.GetActor();

	if (HitActor && HitActor->Implements<UInteractableInterface>())
	{
		FocusedActor = HitActor;
	}
	else
	{
		FocusedActor = nullptr;
	}

}

void AAarav::Interact()
{
	// Priority 1: Check for interactable actors (gates, NPCs, etc.)
	// Use FocusedActor from camera trace
	if (FocusedActor && FocusedActor->Implements<UInteractableInterface>())
	{
		UE_LOG(LogTemp, Error, TEXT("=== INTERACTING WITH:  %s ==="), *FocusedActor->GetName());
		IInteractableInterface::Execute_Interact(FocusedActor, this);
		return; // STOP HERE - Don't check weapons or arm/disarm
	}

	// Priority 2: Check for overlapped weapons (from overlap, not trace)
	AWeapon* OverlappedWeapon = Cast<AWeapon>(OverlappedItem);
	if (OverlappedWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("Equipping weapon:   %s"), *OverlappedWeapon->GetName());
		EquipWeapon(OverlappedWeapon);
		return; // STOP HERE - Don't check arm/disarm
	}

	// Priority 3: Arm/Disarm weapon (only if no gate or weapon interaction)
	if (CanDisarm())
	{
		UE_LOG(LogTemp, Warning, TEXT("Disarming weapon"));
		Disarm();
		return;
	}
    
	if (CanArm())
	{
		UE_LOG(LogTemp, Warning, TEXT("Arming weapon"));
		Arm();
		return;
	}
    
	UE_LOG(LogTemp, Verbose, TEXT("No interaction available"));
}

void AAarav::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	CharacterState = ECharacterState::ECS_OneHandWeaponEquipped;
	OverlappedItem = nullptr;
	EquippedWeapon = Weapon;
}

void AAarav::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AAarav::FindNearestEnemyForAttack()
{
	CombatTarget = nullptr;
    
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), Enemies);
    
	AActor* NearestEnemy = nullptr;
	float NearestDistance = AttackRange;
    
	const FVector PlayerLocation = GetActorLocation();
	const FVector PlayerForward = GetActorForwardVector();
    
	for (AActor* Enemy : Enemies)
	{
		if (! Enemy) continue;
        
		AEnemy* EnemyClass = Cast<AEnemy>(Enemy);
		if (!EnemyClass) continue;
        
		// CRITICAL: Check BOTH IsDead() AND health
		if (EnemyClass->IsDead())
		{
			UE_LOG(LogTemp, Verbose, TEXT("❌ Enemy is DEAD (state): %s"), *Enemy->GetName());
			continue;
		}
        
		// Additional safety:  check health directly
		if (EnemyClass->GetAttributeComponent() && EnemyClass->GetAttributeComponent()->GetHealthPercentage() <= 0.f)
		{
			UE_LOG(LogTemp, Verbose, TEXT("❌ Enemy has 0 health:  %s"), *Enemy->GetName());
			continue;
		}
        
		const FVector ToEnemy = Enemy->GetActorLocation() - PlayerLocation;
		const float Distance = ToEnemy. Size();
        
		if (Distance > AttackRange) continue;
        
		const float DotProduct = FVector:: DotProduct(PlayerForward, ToEnemy.GetSafeNormal());
		if (DotProduct < -0.5f) continue;
        
		if (Distance < NearestDistance)
		{
			NearestDistance = Distance;
			NearestEnemy = Enemy;
		}
	}
    
	if (NearestEnemy)
	{
		CombatTarget = NearestEnemy;
		UE_LOG(LogTemp, Warning, TEXT("✓ Found ALIVE target: %s"), *NearestEnemy->GetName());
	}
	else
	{
		CombatTarget = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("✗ No alive enemy - attacking without warp"));
	}
}

void AAarav::AddItem(FName ItemId)
{
	InventoryItems.Add(ItemId);
	UE_LOG(LogTemp, Warning, TEXT("Item Added: %s"), *ItemId.ToString());
}

bool AAarav::HasItem(FName ItemId) const
{
	return InventoryItems.Contains(ItemId);
}

void AAarav::RemoveItem(FName ItemId)
{
	InventoryItems.Remove(ItemId);
	UE_LOG(LogTemp, Warning, TEXT("Item Removed: %s"), *ItemId.ToString());
}


void AAarav::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

bool AAarav::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped;
}


bool AAarav::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped;
}

bool AAarav::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState == ECharacterState::ECS_Unequipped && EquippedWeapon;
}

void AAarav::Disarm()
{
	PlayEquipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void AAarav::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_OneHandWeaponEquipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

bool AAarav::IsOccupied()
{
	return ActionState != EActionState::EAS_Unoccupied;
}

bool AAarav::HasEnoughStamina()
{
	if (AttributeComponent && AttributeComponent->GetStamina() > AttributeComponent->GetDodgeCost())
	{
		return true;
	}
	return false;
}

void AAarav::AttachWeaponToBack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void AAarav::AttachWeaponToHand()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void AAarav::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AAarav::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AAarav::DodgeEnd()
{
	Super::DodgeEnd();

	ActionState = EActionState::EAS_Unoccupied;
}

void AAarav::InitializeMainOverlayWidget()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		AGameplayMainHUD* MainHUD = Cast<AGameplayMainHUD>(PlayerController->GetHUD());
		if (MainHUD)
		{
			MainOverlay = MainHUD->GetMainOverlay();
			if (MainOverlay && AttributeComponent)
			{
				MainOverlay->SetHealthBarPercent(AttributeComponent->GetHealthPercentage());
				MainOverlay->SetStaminaBarPercent(1.f);
				MainOverlay->SetGold(0);
				MainOverlay->SetSouls(0);
			}
		}
	}
}

void AAarav::SetHUDHealth()
{
	if (MainOverlay && AttributeComponent)
	{
		MainOverlay->SetHealthBarPercent(AttributeComponent->GetHealthPercentage());
	}
}

void AAarav::Die_Implementation()
{
	Super::Die_Implementation();

	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();

	UGameOverWidget* GameOverWidget = CreateWidget<UGameOverWidget>(GetWorld(), GameOverWidgetClass);
	if (GameOverWidget)
	{
		GameOverWidget->AddToViewport();
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(GameOverWidget->TakeWidget());
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true;
		}
	}
}

void AAarav::ClearCombatTarget()
{
	CombatTarget = nullptr;
	UE_LOG(LogTemp, Warning, TEXT("Combat target cleared"));
}

void AAarav::SetFocusedActorManually(AActor* Actor)
{
	FocusedActor = Actor;
	UE_LOG(LogTemp, Error, TEXT("✓ FocusedActor MANUALLY SET to: %s"), Actor ?  *Actor->GetName() : TEXT("nullptr"));
}

void AAarav::ClearFocusedActor()
{
	FocusedActor = nullptr;
	UE_LOG(LogTemp, Warning, TEXT("✓ FocusedActor CLEARED"));
}
