// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Character/AaravAnimInstance.h"
#include "Public/Character/Aarav.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UAaravAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	AaravCharacter = Cast<AAarav>(TryGetPawnOwner());

	if (AaravCharacter) {
		AaravCharacterMovement = AaravCharacter->GetCharacterMovement();
	}
}

void UAaravAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (AaravCharacterMovement) {
			
		GroundSpeed = UKismetMathLibrary::VSizeXY(AaravCharacterMovement->Velocity);
		isFalling = AaravCharacterMovement->IsFalling();
		IsSprinting = GroundSpeed > 300.f;

		CharacterState = AaravCharacter->GetCharacterState();
		ActionState = AaravCharacter->GetActionState();
		DeathPose = AaravCharacter->GetDeathPose();

	}
}
