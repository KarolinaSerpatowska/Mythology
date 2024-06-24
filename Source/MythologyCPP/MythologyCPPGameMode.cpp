// Copyright Epic Games, Inc. All Rights Reserved.

#include "MythologyCPPGameMode.h"
#include "Player/MythologyCPPCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Player/MyPlayerController.h"

AMythologyCPPGameMode::AMythologyCPPGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Player/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<AMyPlayerController> PlayerControllerBPClass(TEXT("/Game/Blueprints/Player/BP_MyPlayerController"));
	if (PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}
