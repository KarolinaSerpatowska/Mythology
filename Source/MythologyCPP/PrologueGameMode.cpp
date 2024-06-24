// Copyright by Karolina Serpatowska All rights reserved


#include "PrologueGameMode.h"
#include "Player/MyPlayerController.h"


APrologueGameMode::APrologueGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_ProloguePlayer"));
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