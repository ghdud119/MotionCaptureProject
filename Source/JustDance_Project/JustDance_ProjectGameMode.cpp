// Copyright Epic Games, Inc. All Rights Reserved.

#include "JustDance_ProjectGameMode.h"
#include "JustDance_ProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AJustDance_ProjectGameMode::AJustDance_ProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
