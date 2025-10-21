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
#include "Components/BoxComponent.h"
#include "Items/Weapon.h"
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
	
}

// Called every frame
void AAarav::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PerformInteractionTrace();

}

// Called to bind functionality to input
void AAarav::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAarav::Move);
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAarav::Look);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::Jump);
		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Started, this, &AAarav::SprintStart);
		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &AAarav::SprintCompleted);
		EnhancedInput->BindAction(InteractionAction, ETriggerEvent::Triggered, this, &AAarav::Interact);
		EnhancedInput->BindAction(AttackAction, ETriggerEvent::Triggered, this , &AAarav::Attack);
	}

}

void AAarav::GetHit_Implementation(const FVector& ImpactPoint)
{
	Super::GetHit_Implementation(ImpactPoint);

	PlayHitSound(ImpactPoint);
	SpawnHitParticle(ImpactPoint);
}


void AAarav::Move(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	
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
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void AAarav::PerformInteractionTrace()
{
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
	if (FocusedActor && FocusedActor->Implements<UInteractableInterface>())
	{
		IInteractableInterface::Execute_Interact(FocusedActor, this);
	}

	AWeapon* OverlappedWeapon = Cast<AWeapon>(OverlappedItem);

	if (OverlappedWeapon)
	{
		EquipWeapon(OverlappedWeapon);
	}
	else
	{
		if(CanDisarm())
		{
			Disarm();
		}
		else if(CanArm())
		{
			Arm();
		}
	}
}

void AAarav::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	CharacterState = ECharacterState::ECS_OneHandWeaponEquipped;
	OverlappedItem = nullptr;
	EquippedWeapon = Weapon;
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

