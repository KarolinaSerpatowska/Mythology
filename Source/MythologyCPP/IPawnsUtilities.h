// Copyright by Karolina Serpatowska All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IPawnsUtilities.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIPawnsUtilities : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MYTHOLOGYCPP_API IIPawnsUtilities
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "My utility functions")
		USkeletalMeshComponent* GetCharacterMesh();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "My utility functions")
		AWeapon* GetWeapon();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "My utility functions")
		bool CanUseAbilities();
};
