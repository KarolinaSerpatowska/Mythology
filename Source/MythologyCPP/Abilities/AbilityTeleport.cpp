// Copyright by Karolina Serpatowska All rights reserved


#include "AbilityTeleport.h"
#include "../IPawnsUtilities.h"

AAbilityTeleport::AAbilityTeleport()
{

}

void AAbilityTeleport::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->GetClass()->ImplementsInterface(UIPawnsUtilities::StaticClass()))
	{
		weapon = IIPawnsUtilities::Execute_GetWeapon(GetOwner());
	}
}

void AAbilityTeleport::AttemptUseAbility()
{
	weapon = IIPawnsUtilities::Execute_GetWeapon(GetOwner());
	if (weapon && weapon->CanTeleportToWeapon() && !weapon->GetOutOfBoundary())
	{
		if (GetOwner()->GetClass()->ImplementsInterface(UIPawnsUtilities::StaticClass()) && IIPawnsUtilities::Execute_CanUseAbilities(GetOwner()))
		{
			UseAbility();
		}
		
	}

	UE_LOG(LogTemp, Warning, TEXT("no ability"));
}

void AAbilityTeleport::UseAbility()
{
	AAbility::UseAbility();

	if (weapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("WEAPON NICE"));
		GetOwner()->SetActorLocation(weapon->GetActorLocation());
	}

}

