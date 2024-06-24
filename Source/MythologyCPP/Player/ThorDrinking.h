// Copyright by Karolina Serpatowska All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "IPlayerInput.h"
#include "MyPlayerController.h"
#include "ThorDrinking.generated.h"


class USpringArmComponent;
class UCameraComponent;

UCLASS()
class MYTHOLOGYCPP_API AThorDrinking : public ACharacter, public IIPlayerInput
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AThorDrinking();

	UPROPERTY(EditAnywhere, Category = UI)
		TSubclassOf<class UUserWidget> HUDClass;

	AMyPlayerController* playerController;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FollowCamera;

private:
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UUserWidget* HUDWidgetInstance;

public:	
	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

};
