// Copyright by Karolina Serpatowska All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IPlayerInput.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIPlayerInput : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MYTHOLOGYCPP_API IIPlayerInput
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Player Character Actions")
		void Move(const FVector2D& Value);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Player Character Actions")
		void ClimbMove(const FVector2D& Value);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Player Character Actions")
		void Look(const FVector2D& Value);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Player Character Actions")
		void Jump();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Player Character Actions")
		void StopJump();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Player Character Actions")
		void Sprint();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Player Character Actions")
		void StopSprint();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Player Character Actions")
		void SwitchWalkRun();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Player Character Actions")
		void StartAim();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Player Character Actions")
		void StopAim();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Player Character Actions")
		void OngoingAim();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Player Character Actions")
		void ThrowWeapon(float value);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Player Character Actions")
		void StartChargeWeapon();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Player Character Actions")
		void Interact();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Player Character Actions")
		void FirstAbility();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Player Character Actions")
		void SecondAbility();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Player Character Actions")
		void StartClimbing();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Player Character Actions")
		void Pause();
};
