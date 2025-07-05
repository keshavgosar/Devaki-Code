// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AaravAnimInstance.generated.h"

class AAarav;
class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class DEVAKIVILLAGE_API UAaravAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime);

	UPROPERTY(BlueprintReadOnly)
	AAarav* AaravCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	UCharacterMovementComponent* AaravCharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool isFalling;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool IsSprinting;
};
