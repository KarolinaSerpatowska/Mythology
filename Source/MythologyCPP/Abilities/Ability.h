// Copyright by Karolina Serpatowska All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ability.generated.h"

UCLASS()
class MYTHOLOGYCPP_API AAbility : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAbility();

private:
	USkeletalMeshComponent* mesh;

	UAnimInstance* characterAnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montages", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* abilityMontage;

	UFUNCTION(BlueprintCallable)
		bool CanUseAbility();

	UFUNCTION(BlueprintCallable)
		bool FinalUseAllowanceCheck();

	UFUNCTION(BlueprintCallable)
		bool IsUsingAbilityRestricted();

	UFUNCTION(BlueprintCallable)
		bool IsUsingAbilityPrerequisitesFulffiled();

	UFUNCTION()
		void Event_OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PlaySomeMontage(UAnimMontage* MontageToPlay);

public:

	UFUNCTION(BlueprintCallable)
		virtual void AttemptUseAbility();

	UFUNCTION(BlueprintCallable)
		virtual void UseAbilityPreStep();

	UFUNCTION(BlueprintCallable)
		virtual void UseAbility();

	UFUNCTION(BlueprintCallable)
		virtual void EndAbility();
};
