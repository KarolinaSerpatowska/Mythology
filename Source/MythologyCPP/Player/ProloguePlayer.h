// Copyright by Karolina Serpatowska All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "IPlayerInput.h"
#include "MyPlayerController.h"
#include "ProloguePlayer.generated.h"


class USpringArmComponent;
class UCameraComponent;

UCLASS()
class MYTHOLOGYCPP_API AProloguePlayer : public ACharacter, public IIPlayerInput
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AProloguePlayer();

private:
	AMyPlayerController* playerController;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USphereComponent* interactionSphereTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = test, meta = (AllowPrivateAccess = "true"))
		TArray<AActor*> interactables;

	UFUNCTION(BlueprintCallable)
		void SphereTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
		void SphereTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Move_Implementation(const FVector2D& Value) override;
	void Look_Implementation(const FVector2D& Value) override;

	void Interact_Implementation() override;


public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

};
