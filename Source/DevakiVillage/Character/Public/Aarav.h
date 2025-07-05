// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Aarav.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;

UCLASS()
class DEVAKIVILLAGE_API AAarav : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAarav();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* InteractionAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* AaravMappingContext;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float WalkSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float SprintSpeed = 600.f;

	AActor* FocusedActor;
	float TraceDistance = 400.f;

	UPROPERTY(BlueprintReadOnly)
	TSet<FName> InventoryItems;

	UFUNCTION(BlueprintCallable)
	void AddItem(FName ItemId);

	UFUNCTION(BlueprintCallable)
	bool HasItem(FName ItemId) const;

	UFUNCTION(BlueprintCallable)
	void RemoveItem(FName ItemId);

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void SprintStart(const FInputActionValue& Value);
	void SprintCompleted(const FInputActionValue& Valuw);

	void PerformInteractionTrace();
	void Interact();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
