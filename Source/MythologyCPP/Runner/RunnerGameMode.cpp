// Copyright by Karolina Serpatowska All rights reserved


#include "RunnerGameMode.h"
#include "../Player/MyPlayerController.h"


ARunnerGameMode::ARunnerGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Runner/BP_RunnerPlayer"));
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
