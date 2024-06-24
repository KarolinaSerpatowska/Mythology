// Copyright by Karolina Serpatowska All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Player/IPlayerInput.h"
#include "../Player/MyPlayerController.h"
#include "RunnerPlayer.generated.h"


class USpringArmComponent;
class UCameraComponent;

DECLARE_DELEGATE(FOnDie)

UCLASS()
class MYTHOLOGYCPP_API ARunnerPlayer : public ACharacter, public IIPlayerInput
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARunnerPlayer();

	UPROPERTY(EditAnywhere, Category = UI)
		TSubclassOf<class UUserWidget> HUDClass;

	UPROPERTY(EditAnywhere, Category = UI)
		TSubclassOf<class UUserWidget> EndUIClass;

private:
	AMyPlayerController* playerController;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FollowCamera;

	bool canMove = true;

	int points = 0;

	UPROPERTY()
		UUserWidget* HUDWidgetInstance;

	UPROPERTY()
		UUserWidget* EndUIWidgetInstance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void MoveForward();

	void Move_Implementation(const FVector2D& Value) override;
	void Pause_Implementation() override;

	void Die();

public:
	FOnDie OnDieDelegate;

	UFUNCTION(BlueprintCallable)
		void GivePoints();

	UFUNCTION(BlueprintCallable)
		int GetPoints();

	UFUNCTION(BlueprintCallable)
		void EndRun();

	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

};
