// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Items/Item.h"

#include "Character/Aarav.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>("ItemMesh");
	RootComponent = ItemMesh;

	ItemSphere = CreateDefaultSubobject<USphereComponent>("ItemSphere");
	ItemSphere->SetupAttachment(ItemMesh);

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	ItemSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	ItemSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
	
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.0f, 0.0f, 0.f));
	}

}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	AAarav* AaravCharacter = Cast<AAarav>(OtherActor);

	if (AaravCharacter)
	{
		AaravCharacter->SetOverlappedItem(this);
	}
	
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AAarav* AaravCharacter = Cast<AAarav>(OtherActor);

	if (AaravCharacter)
	{
		AaravCharacter->SetOverlappedItem(nullptr);
	}
	
}

