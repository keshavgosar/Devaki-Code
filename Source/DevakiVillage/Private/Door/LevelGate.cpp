// Fill out your copyright notice in the Description page of Project Settings.

#include "Door/LevelGate.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Character/Aarav.h"
#include "Enemy/Enemy.h"
#include "HUD/MainOverlay.h"
#include "Kismet/GameplayStatics.h"

ALevelGate::ALevelGate()
{
    PrimaryActorTick.bCanEverTick = true;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);

    PillerArchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillerArchMesh"));
    PillerArchMesh->SetupAttachment(SceneRoot);
    PillerArchMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PillerArchMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    GateLeftMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateLeftMesh"));
    GateLeftMesh->SetupAttachment(PillerArchMesh);
    GateLeftMesh->SetCollisionEnabled(ECollisionEnabled:: QueryOnly);
    GateLeftMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    GateRightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateRightMesh"));
    GateRightMesh->SetupAttachment(PillerArchMesh);
    GateRightMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    GateRightMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(SceneRoot);
    TriggerBox->SetBoxExtent(FVector(200.f, 200.f, 200.f));
    TriggerBox->SetCollisionEnabled(ECollisionEnabled:: QueryOnly);
    TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void ALevelGate::BeginPlay()
{
    Super::BeginPlay();
    
    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ALevelGate::OnBoxBeginOverlap);
    TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ALevelGate::OnBoxEndOverlap);
}

void ALevelGate:: Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Debug draw trigger box
    FVector BoxLocation = TriggerBox->GetComponentLocation();
    FVector BoxExtent = TriggerBox->GetScaledBoxExtent();
    DrawDebugBox(GetWorld(), BoxLocation, BoxExtent, FColor::Green, false, -1.f, 0, 2.f);
}

UMainOverlay* ALevelGate::GetPlayerOverlay()
{
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        AAarav* Player = Cast<AAarav>(PlayerController->GetPawn());
        if (Player)
        {
            return Player->GetMainOverlay();
        }
    }
    return nullptr;
}

void ALevelGate::ShowInteractionPrompt()
{
    UMainOverlay* Overlay = GetPlayerOverlay();
    if (Overlay)
    {
        Overlay->ShowInteractionPrompt(InteractText);
        UE_LOG(LogTemp, Error, TEXT("=== SHOWING INTERACTION PROMPT ON HUD ==="));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Could not get MainOverlay! "));
    }
}

void ALevelGate::HideInteractionPrompt()
{
    UMainOverlay* Overlay = GetPlayerOverlay();
    if (Overlay)
    {
        Overlay->HideInteractionPrompt();
    }
}

void ALevelGate::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AAarav* Player = Cast<AAarav>(OtherActor);
    if (Player)
    {
        UE_LOG(LogTemp, Error, TEXT("=== PLAYER ENTERED GATE AREA ==="));
        
        bPlayerInRange = true;
        PlayerInTrigger = Player;
        
        // CRITICAL:  Manually set this gate as focused actor
        Player->SetFocusedActorManually(this);
        
        if (CanTravelToNextLevel())
        {
            ShowInteractionPrompt();
        }
        else
        {
            UMainOverlay* Overlay = GetPlayerOverlay();
            if (Overlay)
            {
                Overlay->ShowInteractionPrompt("Defeat all enemies first!");
            }
        }
    }
}

void ALevelGate::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    AAarav* Player = Cast<AAarav>(OtherActor);
    if (Player)
    {
        UE_LOG(LogTemp, Warning, TEXT("=== PLAYER LEFT GATE AREA ==="));
        
        bPlayerInRange = false;
        PlayerInTrigger = nullptr;
        
        // CRITICAL: Clear focused actor
        Player->ClearFocusedActor();
        
        HideInteractionPrompt();
    }
}

void ALevelGate:: Interact_Implementation(AActor* InteractingActor)
{
    AAarav* Player = Cast<AAarav>(InteractingActor);
    if (Player && bPlayerInRange)
    {
        if (CanTravelToNextLevel())
        {
            UE_LOG(LogTemp, Error, TEXT("=== TRAVELING TO LEVEL:  %s ==="), *LevelToLoad. ToString());
            HideInteractionPrompt();
            LoadNextLevel();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Cannot travel yet - enemies still alive"));
        }
    }
}

bool ALevelGate:: CanTravelToNextLevel()
{
    if (! bRequireAllEnemiesDead)
    {
        return true;
    }

    TArray<AActor*> Enemies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), Enemies);

    for (AActor* Actor : Enemies)
    {
        AEnemy* Enemy = Cast<AEnemy>(Actor);
        if (Enemy && !Enemy->IsDead())
        {
            return false;
        }
    }

    return true;
}

void ALevelGate::LoadNextLevel()
{
    if (LevelToLoad != NAME_None)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
        if (PC)
        {
            PC->PlayerCameraManager->StartCameraFade(0.f, 1.f, FadeOutDuration, FLinearColor::Black, false, true);
        }

        // Delay level load for fade
        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(TimerHandle, [this]()
        {
            UGameplayStatics::OpenLevel(this, LevelToLoad);
        }, FadeOutDuration, false);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("LevelToLoad is not set!"));
    }
}