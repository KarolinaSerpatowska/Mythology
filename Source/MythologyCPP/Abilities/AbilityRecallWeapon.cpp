// Copyright by Karolina Serpatowska All rights reserved


#include "AbilityRecallWeapon.h"
#include "../IPawnsUtilities.h"

void AAbilityRecallWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->GetClass()->ImplementsInterface(UIPawnsUtilities::StaticClass()))
	{
		weapon = IIPawnsUtilities::Execute_GetWeapon(GetOwner());
	}
}

void AAbilityRecallWeapon::AttemptUseAbility()
{
	weapon = IIPawnsUtilities::Execute_GetWeapon(GetOwner());
	if (weapon && weapon->CanTeleportToWeapon())
	{
		if (GetOwner()->GetClass()->ImplementsInterface(UIPawnsUtilities::StaticClass()) && IIPawnsUtilities::Execute_CanUseAbilities(GetOwner()))
		{
			UseAbility();
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("no ability"));
}

void AAbilityRecallWeapon::UseAbility()
{
	AAbility::UseAbility();

	if (weapon)
	{
		weapon->RecallWeapon();
	}
}

