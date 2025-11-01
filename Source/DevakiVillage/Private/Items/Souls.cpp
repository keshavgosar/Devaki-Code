// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Souls.h"

#include "Interfaces/PickupInterface.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraVariant.h"
#include "Kismet/KismetSystemLibrary.h"

void ASouls::BeginPlay()
{
	Super::BeginPlay();

	const FVector Start = GetActorLocation();
	const FVector End = Start - FVector(0.f, 0.f, 2000.f);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	
	FHitResult HitResult;
	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		Start,
		End,
		ObjectTypes ,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true
		);
	
	DesiredZ = HitResult.ImpactPoint.Z + 100.f;
	
}

void ASouls::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Location = GetActorLocation();

	if (Location.Z > DesiredZ)
	{
		const double NewZ = DriftRate * DeltaTime;
		AddActorWorldOffset(FVector(0.f, 0.f, NewZ));
	}
}


void ASouls::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);

	if (PickupInterface)
	{
		PickupInterface->AddSouls(this);
	}

	SpawnPickupSystem();
	SpawnPickupSound();

	Destroy();
}
