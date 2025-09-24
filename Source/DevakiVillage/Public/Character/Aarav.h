// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Character/CharacterTypes.h"
#include "Aarav.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class AItem;
class AWeapon;


UCLASS()
class DEVAKIVILLAGE_API AAarav : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAarav();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

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
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* AaravMappingContext;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWeapon* EquippedWeapon;

	/*
	 * Animation Montages
	 */
	
	UPROPERTY(EditAnywhere, Category = "Anim Montages | Attack Montage")
	UAnimMontage* AaravAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Anim Montages | Attack Montage")
	UAnimMontage* EquipMontage;

	void PlayAttackMontage();
	void PlayEquipMontage(FName SectionName);

	bool CanDisarm();
	bool CanArm();

	UFUNCTION(BlueprintCallable)
	void Arm();
	
	UFUNCTION(BlueprintCallable)
	void Disarm();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	UPROPERTY(EditAnywhere, Category = "Movement")
	float WalkSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float SprintSpeed = 600.f;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void SprintStart(const FInputActionValue& Value);
	void SprintCompleted(const FInputActionValue& Value);
	void Attack(const FInputActionValue& Value);

	void PerformInteractionTrace();
	void Interact();

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappedItem;

public:

	UPROPERTY(VisibleInstanceOnly)
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

	FORCEINLINE void SetOverlappedItem(AItem* Item) { OverlappedItem = Item; }

	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

};
