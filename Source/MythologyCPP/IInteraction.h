// Copyright by Karolina Serpatowska All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IInteraction.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIInteraction : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MYTHOLOGYCPP_API IIInteraction
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
		void OnInteraction(ACharacter* player);
};
