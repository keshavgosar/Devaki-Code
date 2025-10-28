// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "Character/CharacterTypes.h"
#include "Interfaces/PickupInterface.h"
#include "Aarav.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class AItem;
class UMainOverlay;

UCLASS()
class DEVAKIVILLAGE_API AAarav : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	AAarav();

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Jump Function
	virtual void Jump() override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* HitActor) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void SetOverlappingItem(class AItem* Item) override;
	virtual void AddSouls(class ASouls* Soul) override;
	virtual void AddGold(class ATreasure* Treasure) override;

protected:

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;
	
	virtual void AttackEnd() override;
	virtual void DodgeEnd() override;

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
	UInputAction* DodgeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* AaravMappingContext;

	/*
	 * Animation Montages
	 */

	UPROPERTY(EditAnywhere, Category = "Anim Montages | Attack Montage")
	UAnimMontage* EquipMontage;

	/*
	 * Play Attack Montage
	 */
	void PlayEquipMontage(const FName& SectionName);

	virtual bool CanAttack() override;

	bool CanDisarm();
	bool CanArm();
	void Disarm();
	void Arm();

	bool IsOccupied();
	bool HasEnoughStamina();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();
	
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();

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
	void Dodge(const FInputActionValue& Value);

	void PerformInteractionTrace();
	void Interact();

	void EquipWeapon(AWeapon* Weapon);

	void InitializeMainOverlayWidget();
	void SetHUDHealth();

	virtual void Die() override;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappedItem;

	UPROPERTY()
	UMainOverlay* MainOverlay;

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

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

	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

	FORCEINLINE EActionState GetActionState() const { return ActionState; }

};
