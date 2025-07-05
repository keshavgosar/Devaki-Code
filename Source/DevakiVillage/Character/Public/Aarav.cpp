// Fill out your copyright notice in the Description page of Project Settings.


#include "Aarav.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "InteractableInterface.h"

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

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(AaravMappingContext, 0);
		}
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

void AAarav::Move(const FInputActionValue& Value)
{
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
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AAarav::SprintCompleted(const FInputActionValue& Valuw)
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
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
	}

}

