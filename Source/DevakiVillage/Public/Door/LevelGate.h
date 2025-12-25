// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractableInterface.h"
#include "LevelGate.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UWidgetComponent;

UCLASS()
class DEVAKIVILLAGE_API ALevelGate : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:    
	ALevelGate();
	virtual void Tick(float DeltaTime) override;

	// Interactable Interface
	virtual void Interact_Implementation(AActor* InteractingActor) override;

	UPROPERTY()
	class AAarav* PlayerInTrigger = nullptr;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:

	UPROPERTY(EditAnywhere, Category = "Level Gate")
	float FadeOutDuration = 2.f;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* PillerArchMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* GateLeftMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* GateRightMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* TriggerBox;

	// Level to load
	UPROPERTY(EditAnywhere, Category = "Level Gate")
	FName LevelToLoad;

	UPROPERTY(EditAnywhere, Category = "Level Gate")
	FString InteractText = "Press E to Enter";

	UPROPERTY(EditAnywhere, Category = "Level Gate")
	bool bRequireAllEnemiesDead = false;

	UPROPERTY()
	bool bPlayerInRange = false;

	void LoadNextLevel();
	bool CanTravelToNextLevel();
	class UMainOverlay* GetPlayerOverlay();
	void ShowInteractionPrompt();
	void HideInteractionPrompt();

};
