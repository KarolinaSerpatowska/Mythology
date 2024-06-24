// Copyright by Karolina Serpatowska All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "Components/TimelineComponent.h"
#include "MyPlayerController.generated.h"


DECLARE_DELEGATE(FOnStopClimbing)
DECLARE_DELEGATE(FOnStartClimbing)


class UInputMappingContext;

/**
 * 
 */
UCLASS()
class MYTHOLOGYCPP_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()


public:
	AMyPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
		UCurveFloat* chargeCurve;

	FTimeline chargeTimeline;

	float chargeValue = 0;

	UFUNCTION(BlueprintCallable)
		void HoldCharge(float value);

	FOnStartClimbing OnStartClimbingDelegate;
	FOnStopClimbing OnStopClimbingDelegate;

	UPROPERTY(EditAnywhere, Category = UI)
		TSubclassOf<class UUserWidget> pauseMenuClass;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	void AddInputMappingContext(UInputMappingContext* contextToAdd, int32 inPriority);
	void RemoveInputMappingContext(UInputMappingContext* contextToRemove);

	void StartClimbing();
	void StopClimbing();

	UFUNCTION(BlueprintCallable)
		void CallMove(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
		void CallClimbMove(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
		void CallJump();

	UFUNCTION(BlueprintCallable)
		void CallStopJump();

	UFUNCTION(BlueprintCallable)
		void CallLook(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
		void CallSprint();

	UFUNCTION(BlueprintCallable)
		void CallSwitchWalkRun();

	UFUNCTION(BlueprintCallable)
		void CallStopSprint();

	UFUNCTION(BlueprintCallable)
		void CallStartCharge();

	UFUNCTION(BlueprintCallable)
		void CallStopCharge(const FInputActionInstance& Instance);

	UFUNCTION(BlueprintCallable)
		void CallCharging(const FInputActionInstance& Instance);

	UFUNCTION(BlueprintCallable)
		void CallStartAiming();

	UFUNCTION(BlueprintCallable)
		void CallZoomAim(const FInputActionInstance& Instance);

	UFUNCTION(BlueprintCallable)
		void CallStopAim(const FInputActionInstance& Instance);

	UFUNCTION(BlueprintCallable)
		void CallInteraction();

	UFUNCTION(BlueprintCallable)
		void CallStartClimb();

	UFUNCTION(BlueprintCallable)
		void CallFirstAbility();

	UFUNCTION(BlueprintCallable)
		void CallSecondAbility();

	UFUNCTION(BlueprintCallable)
		void PauseGame();

private:
	APawn* possessedPawn;

	UPROPERTY()
		UUserWidget* pauseWidgetInstance;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputMappingContext* ClimbingMappingContext;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* ClimbMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* LookAction;

	///** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* ThrowAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* SwitchWalkRunAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* ClimbAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* PauseGameAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* FirstAbilityAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* SecondAbilityAction;

	public:
		UFUNCTION(BlueprintCallable)
			void ChangePawn();
};
