// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseDoor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Character/Public/Aarav.h"

// Sets default values
ABaseDoor::ABaseDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"));

	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(FName("Door Mesh"));
	DoorFrame->SetupAttachment(RootComponent);

	DoorPivot = CreateDefaultSubobject<USceneComponent>(FName("Door Pivot"));
	DoorPivot->SetupAttachment(RootComponent);

	DoorInner = CreateDefaultSubobject<UStaticMeshComponent>(FName("Door Inner Mesh"));
	DoorInner->SetupAttachment(DoorPivot);

}

// Called when the game starts or when spawned
void ABaseDoor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseDoor::OpenDoor()
{
	if (!bIsOpen)
	{
		bIsOpen = true;

		UE_LOG(LogTemp, Warning, TEXT("Door Is Open"));

		TargetRotation = FRotator(0.f, 90.f, 0.f); // You can also do += if you want toggle
		bIsOpening = true;
	}

}

// Called every frame
void ABaseDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsOpening)
	{
		FRotator CurrentRotation = DoorPivot->GetRelativeRotation();
		FRotator NewRotation = FMath::RInterpConstantTo(CurrentRotation, TargetRotation, DeltaTime, 90.f);
		DoorPivot->SetRelativeRotation(NewRotation);

		// Use CurrentRotation or updated value after applying the rotation
		if (NewRotation.Equals(TargetRotation, 0.1f))
		{
			bIsOpening = false;
			
		}
	}

}

void ABaseDoor::Interact_Implementation(AActor* Interactor)
{
	if (bIsOpen) return;

	switch (DoorType)
	{

	case EDoorType::Normal:
	{
		OpenDoor();

		break;
	}

	case EDoorType::Key:
	{
		AAarav* Aarav = Cast<AAarav>(Interactor);

		if (Aarav && Aarav->HasItem(RequiredKeyTag))
		{
			OpenDoor();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("You Required Key To Open This Gate %s"), *RequiredKeyTag.ToString());
		}

		break;
	}

	case EDoorType::Puzzle:
	{
		if (bPuzzleSolved)
		{
			OpenDoor();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("The Puzzle is not solved"));
		}

		break;
	}

	}
}

FText ABaseDoor::GetInteractionText_Implementation()
{

	switch (DoorType)
	{

	case EDoorType::Normal:

		return FText::FromString("Open Door");
	
	case EDoorType::Key:

		return FText::FromString("You Need Key To Open This Door");
	
	case EDoorType::Puzzle:

		return FText::FromString("Solve Puzzle To Open");
		
	default: 

		return FText::FromString("Interact");
		
	}

}

