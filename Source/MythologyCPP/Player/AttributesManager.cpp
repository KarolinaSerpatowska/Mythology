// Copyright by Karolina Serpatowska All rights reserved


#include "AttributesManager.h"

// Sets default values for this component's properties
UAttributesManager::UAttributesManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	
	// ...
}

// Called when the game starts
void UAttributesManager::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);

	world = GetWorld();
	
	stamina = maxStamina;
	StaminaDecreaseDelayDelegate.BindUObject(this, &UAttributesManager::SubtractStamina, valueForDecreasingStamina);
	StaminaIncreaseDelayDelegate.BindUObject(this, &UAttributesManager::AddToStamina, valueForDecreasingStamina);
	
	
}

// Called every frame
void UAttributesManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (recoveringStamina)
	{
		if (stamina == maxStamina)
		{
			if (world) world->GetTimerManager().ClearTimer(StaminaIncreaseDelayHandle);
			recoveringStamina = false;
			SetComponentTickEnabled(false);
		}
	}
}

void UAttributesManager::SubtractStamina(float value)
{
	if (!GetOwner()->GetVelocity().IsZero())
	{
		stamina -= value;
		stamina = FMath::Clamp(stamina, 0, maxStamina);
		if (stamina == 0.f)
		{
			OnNoStaminaDelegate.ExecuteIfBound();
			if (world) world->GetTimerManager().ClearTimer(StaminaDecreaseDelayHandle);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("stamina: %f"), stamina);
}

void UAttributesManager::DecreaseStamina()
{
	if (world)
	{
		world->GetTimerManager().ClearTimer(StaminaIncreaseDelayHandle);
		world->GetTimerManager().SetTimer(StaminaDecreaseDelayHandle, StaminaDecreaseDelayDelegate, staminaDelay, true);
	}
	
}

void UAttributesManager::RecoverStamina()
{
	if(world) world->GetTimerManager().ClearTimer(StaminaDecreaseDelayHandle);
	recoveringStamina = true;
	SetComponentTickEnabled(true);
	world->GetTimerManager().SetTimer(StaminaIncreaseDelayHandle, StaminaIncreaseDelayDelegate, staminaDelay, true);
}

void UAttributesManager::AddToStamina(float value)
{
	stamina += value;
	stamina = FMath::Clamp(stamina, 0, maxStamina);
	UE_LOG(LogTemp, Warning, TEXT("stamina: %f"), stamina);
}