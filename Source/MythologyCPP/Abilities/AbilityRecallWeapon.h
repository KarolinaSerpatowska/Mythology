// Copyright by Karolina Serpatowska All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "../Weapon/Weapon.h"
#include "AbilityRecallWeapon.generated.h"

/**
 * 
 */
UCLASS()
class MYTHOLOGYCPP_API AAbilityRecallWeapon : public AAbility
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	virtual void AttemptUseAbility() override;

public:
	AWeapon* weapon;

	virtual void UseAbility() override;
};
