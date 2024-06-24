// Copyright by Karolina Serpatowska All rights reserved


#include "PickupComponent.h"

// Sets default values for this component's properties
UPickupComponent::UPickupComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UPickupComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UPickupComponent::DealWithPickup()
{
	GetOwner()->Destroy();
}

