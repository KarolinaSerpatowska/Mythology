// Copyright by Karolina Serpatowska All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Ability.h"
#include "AbilitiesManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYTHOLOGYCPP_API UAbilitiesManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAbilitiesManager();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Variables, meta = (AllowPrivateAccess = "true"))
		TArray<AAbility*> availableAbilities;

	UPROPERTY(EditAnywhere, Category = Weapon)
		TArray<TSubclassOf<class AAbility>> knownAbilities;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable)
		void InitAbilities();

	UFUNCTION(BlueprintCallable)
		void UseAbility(int abilityIndex);
};
