// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "BaseDoor.generated.h"

UENUM()
enum class EDoorType : uint8
{
	Normal,
	Key,
	Puzzle
};

UCLASS()
class DEVAKIVILLAGE_API ABaseDoor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OpenDoor();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EDoorType DoorType = EDoorType::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* DoorFrame;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USceneComponent* DoorPivot;

	bool bIsOpening = false;

	FRotator TargetRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* DoorInner;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FName RequiredKeyTag;

	UPROPERTY(BlueprintReadWrite)
	bool bIsOpen = false;

	UPROPERTY(BlueprintReadWrite)
	bool bPuzzleSolved = false;

	//Interface
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionText_Implementation() override;

};
