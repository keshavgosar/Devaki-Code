// Copyright Epic Games, Inc. All Rights Reserved.

#include "DevakiVillageGameMode.h"
#include "DevakiVillageCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADevakiVillageGameMode::ADevakiVillageGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
