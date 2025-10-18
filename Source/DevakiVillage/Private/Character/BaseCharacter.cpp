// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"

#include "Component/AttributeComponent.h"
#include "Components/BoxComponent.h"
#include "Items/Weapon.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Attack()
{
}

void ABaseCharacter::Die()
{
}

void ABaseCharacter::PlayAttackMontage()
{
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	//Get Forward Vector
	const FVector ActorForwardVector = GetActorForwardVector();
	const FVector ImpactPointLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	// Find To hit Vector
	const FVector ToHit = (ImpactPointLowered - GetActorLocation()).GetSafeNormal();

	//Dot Product of these two will return cos theta
	const double CosTheta = FVector::DotProduct(ToHit, ActorForwardVector);
	//We just need the angle so we do inverse of the angle to remove the cos
	double Theta = FMath::Acos(CosTheta);
	//the angle is in radiant so we convert it to degrees.
	Theta = FMath::RadiansToDegrees(Theta);

	//find cross product to get the negative angle so that we can determine if the hit is from left or the right
	const FVector CrossProduct = FVector::CrossProduct(ActorForwardVector, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, FString::Printf(TEXT("Theta: %f"), Theta));
	}

	FName Section("FromBack");

	if (Theta >= -45.0f && Theta < 45.0f)
	{
		Section = FName("FromFront");
	}
	else if (Theta >= -135.0f && Theta < -45.0f)
	{
		Section = FName("FromLeft");
	}
	else if (Theta >= 45.0f && Theta < 135.0f)
	{
		Section = FName("FromRight");	
	}

	PlayHitReactMontage(Section);
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

void ABaseCharacter::AttackEnd()
{
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetBoxComponent())
	{
		EquippedWeapon->GetBoxComponent()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}


