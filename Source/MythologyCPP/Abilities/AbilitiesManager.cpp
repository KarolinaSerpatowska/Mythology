// Copyright by Karolina Serpatowska All rights reserved


#include "AbilitiesManager.h"

// Sets default values for this component's properties
UAbilitiesManager::UAbilitiesManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UAbilitiesManager::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UAbilitiesManager::InitAbilities()
{
	for (auto& ability : availableAbilities)
	{
		if (ability)
		{
			ability->Destroy();
		}
	}

	availableAbilities.Empty();

	for (auto& abilityClass : knownAbilities)
	{
		if (GetWorld())
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			availableAbilities.Add(GetWorld()->SpawnActor<AAbility>(abilityClass, SpawnParams));
		}

	}
}

void UAbilitiesManager::UseAbility(int abilityIndex)
{
	if (availableAbilities[abilityIndex])
	{
		availableAbilities[abilityIndex]->AttemptUseAbility();
	}
}
