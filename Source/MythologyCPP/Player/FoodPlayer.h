// Copyright by Karolina Serpatowska All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "IPlayerInput.h"
#include "MyPlayerController.h"
#include "FoodPlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMinigameFoodEnd);

UCLASS()
class MYTHOLOGYCPP_API AFoodPlayer : public ACharacter, public IIPlayerInput
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFoodPlayer();

	void IncrementTime();

	UPROPERTY(EditAnywhere, Category = UI)
		TSubclassOf<class UUserWidget> HUDClass;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timer, meta = (AllowPrivateAccess = "true"))
	float seconds = 100.f;

	FTimerHandle TimerHandle;

	void DecrementTime();

	UFUNCTION(BlueprintCallable)
		void SphereTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
		void SphereTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UUserWidget* HUDWidgetInstance;


	void Move_Implementation(const FVector2D& Value) override;
	void Jump_Implementation() override;
	void StopJump_Implementation() override;
	void Look_Implementation(const FVector2D& Value) override;
	void Pause_Implementation() override;

	void Sprint_Implementation() override;
	void StopSprint_Implementation() override;

	void Interact_Implementation() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		float defaultRunSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		float defaultSprintSpeed = 1600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		int howManyFoodStopsGame = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		int currentPickupedFood = 0;

	UPROPERTY(BlueprintAssignable)
		FOnMinigameFoodEnd OnMinigameFoodEndDelegate;

	UFUNCTION(BlueprintCallable)
		void StartFoodMinigame();

	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

};