// Copyright by Karolina Serpatowska All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyBlueprintFunctions.generated.h"

/**
 * 
 */
UCLASS()
class MYTHOLOGYCPP_API UMyBlueprintFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "My functions")
	static void WriteStringToFile(FString filePath, FString string, bool& isSuccess, FString& infoMessage);

	UFUNCTION(BlueprintCallable, Category = "My functions")
		static TArray<FString> SplitString(FString text);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Color at coords"), Category = "Get Color")
	static FColor ProcessTexture(UTexture2D* InTexture, int32 PixelX, int32 PixelY);
};
