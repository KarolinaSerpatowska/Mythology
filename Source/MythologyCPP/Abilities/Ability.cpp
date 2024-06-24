// Copyright by Karolina Serpatowska All rights reserved


#include "Ability.h"
#include "../IPawnsUtilities.h"

// Sets default values
AAbility::AAbility()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AAbility::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->GetClass()->ImplementsInterface(UIPawnsUtilities::StaticClass()))
	{
		mesh = IIPawnsUtilities::Execute_GetCharacterMesh(GetOwner());
	}

	if (mesh)
	{
		characterAnimInstance = mesh->GetAnimInstance();
	}

}

void AAbility::AttemptUseAbility()
{
	if (CanUseAbility())
	{
		UseAbilityPreStep();
	}
}

void AAbility::UseAbilityPreStep()
{

	UseAbility();
}

void AAbility::UseAbility()
{
	if (mesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("ANIM CODE HERE"));
		//anims
		if (abilityMontage)
		{
			PlaySomeMontage(abilityMontage);
			characterAnimInstance->OnMontageEnded.AddUniqueDynamic(this, &AAbility::Event_OnMontageEnded);
		}

	}
	UE_LOG(LogTemp, Warning, TEXT("USE ABILITY"));
}

void AAbility::EndAbility()
{
	UE_LOG(LogTemp, Warning, TEXT("END ABILITY"));
}

bool AAbility::CanUseAbility()
{
	if (!FinalUseAllowanceCheck()) return false;
	if (IsUsingAbilityRestricted()) return false;
	if (!IsUsingAbilityPrerequisitesFulffiled()) return false;

	return true;
}

bool AAbility::FinalUseAllowanceCheck()
{
	return true;
}

bool AAbility::IsUsingAbilityRestricted()
{
	return false;
}

bool AAbility::IsUsingAbilityPrerequisitesFulffiled()
{
	return true;
}

void AAbility::PlaySomeMontage(UAnimMontage* MontageToPlay)
{
	if (MontageToPlay && characterAnimInstance)
	{
		characterAnimInstance->Montage_Play(MontageToPlay);
	}
}

void AAbility::Event_OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	EndAbility();
}
