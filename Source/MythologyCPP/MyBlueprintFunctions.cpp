// Copyright by Karolina Serpatowska All rights reserved


#include "MyBlueprintFunctions.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"


void UMyBlueprintFunctions::WriteStringToFile(FString filePath, FString string, bool& isSuccess, FString& infoMessage)
{

	filePath = FPaths::ProjectDir() + "/odpowiedzi.txt";

	if (!FFileHelper::SaveStringToFile(string, *filePath))
	{
		isSuccess = false;
		infoMessage = FString::Printf(TEXT("Failed"));
		return;
	}
	isSuccess = true;
	infoMessage = FString::Printf(TEXT("Success"));
}

TArray<FString> UMyBlueprintFunctions::SplitString(FString text)
{
	TArray<FString> outStringArray;
	text.ParseIntoArray(outStringArray, TEXT(";"), true);
	return outStringArray;
}

FColor UMyBlueprintFunctions::ProcessTexture(UTexture2D* InTexture, int32 PixelX, int32 PixelY)
{
	FTexture2DMipMap* MyMipMap = &InTexture->PlatformData->Mips[0];
	FByteBulkData* RawImageData = &MyMipMap->BulkData;
	FColor* FormatedImageData = static_cast<FColor*>(InTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_ONLY));

	uint32 TextureWidth = MyMipMap->SizeX, TextureHeight = MyMipMap->SizeY;
	FColor PixelColor;

	if (PixelX >= 0 && (uint32)PixelX < TextureWidth && PixelY >= 0 && (uint32)PixelY < TextureHeight)
	{
		PixelColor = FormatedImageData[PixelY * TextureWidth + PixelX];
	}


	RawImageData->Unlock();

	return PixelColor;
}