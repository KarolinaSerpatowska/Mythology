// Copyright by Karolina Serpatowska All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributesManager.generated.h"

DECLARE_DELEGATE(FOnNoStamina)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYTHOLOGYCPP_API UAttributesManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttributesManager();

private:
	FTimerHandle StaminaDecreaseDelayHandle;
	FTimerDelegate StaminaDecreaseDelayDelegate;

	FTimerHandle StaminaIncreaseDelayHandle;
	FTimerDelegate StaminaIncreaseDelayDelegate;
	
	void SubtractStamina(float value);
	void AddToStamina(float value);

	UWorld* world;

	bool recoveringStamina = false;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Attributes, meta = (AllowPrivateAccess = "true"))
	float stamina = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes, meta = (AllowPrivateAccess = "true"))
	float maxStamina = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes, meta = (AllowPrivateAccess = "true"))
	float valueForDecreasingStamina = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes, meta = (AllowPrivateAccess = "true"))
	float staminaDelay = 0.02f;

public:	
	FOnNoStamina OnNoStaminaDelegate;

	UFUNCTION()
	void DecreaseStamina();

	UFUNCTION()
	void RecoverStamina();
	


		
};
