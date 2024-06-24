// Copyright by Karolina Serpatowska All rights reserved


#include "MyPlayerController.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "IPlayerInput.h"

AMyPlayerController::AMyPlayerController()
{

}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	possessedPawn = GetPawn();

	AddInputMappingContext(DefaultMappingContext, 0);

	OnStopClimbingDelegate.BindUObject(this, &ThisClass::StopClimbing);
	OnStartClimbingDelegate.BindUObject(this, &ThisClass::StartClimbing);

	if (chargeCurve)
	{
		FOnTimelineFloat timelineUpdateFunction;
		timelineUpdateFunction.BindUFunction(this, FName("HoldCharge"));
		chargeTimeline.AddInterpFloat(chargeCurve, timelineUpdateFunction);
	}

	if (pauseMenuClass)
	{
		if (!pauseWidgetInstance)
		{
			pauseWidgetInstance = CreateWidget(this, pauseMenuClass);
			if (!pauseWidgetInstance->IsInViewport())
			{
				pauseWidgetInstance->AddToViewport();
				ESlateVisibility visibility;
				visibility = ESlateVisibility::Hidden;
				pauseWidgetInstance->SetVisibility(visibility);
			}
		}
	}

}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AMyPlayerController::CallJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMyPlayerController::CallStopJump);

		//sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AMyPlayerController::CallSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMyPlayerController::CallStopSprint);

		// walk/run
		EnhancedInputComponent->BindAction(SwitchWalkRunAction, ETriggerEvent::Triggered, this, &AMyPlayerController::CallSwitchWalkRun);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPlayerController::CallMove);
		EnhancedInputComponent->BindAction(ClimbMoveAction, ETriggerEvent::Triggered, this, &AMyPlayerController::CallClimbMove);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyPlayerController::CallLook);

		//ChargeThrow
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Started, this, &AMyPlayerController::CallStartCharge);
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Ongoing, this, &AMyPlayerController::CallCharging);
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Completed, this, &AMyPlayerController::CallStopCharge);
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Canceled, this, &AMyPlayerController::CallStopCharge);

		//Aim
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AMyPlayerController::CallStartAiming);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Ongoing, this, &AMyPlayerController::CallZoomAim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &AMyPlayerController::CallStopAim);

		//E
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AMyPlayerController::CallInteraction);

		//Climbing
		EnhancedInputComponent->BindAction(ClimbAction, ETriggerEvent::Started, this, &AMyPlayerController::CallStartClimb);
		
		//pause
		EnhancedInputComponent->BindAction(PauseGameAction, ETriggerEvent::Started, this, &AMyPlayerController::PauseGame);

		//abilities
		//first ability
		EnhancedInputComponent->BindAction(FirstAbilityAction, ETriggerEvent::Triggered, this, &AMyPlayerController::CallFirstAbility);

		//second ability
		EnhancedInputComponent->BindAction(SecondAbilityAction, ETriggerEvent::Triggered, this, &AMyPlayerController::CallSecondAbility);
	}
}

void AMyPlayerController::AddInputMappingContext(UInputMappingContext* contextToAdd, int32 inPriority)
{
	if (!contextToAdd) return;

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(contextToAdd, inPriority);
	}
}

void AMyPlayerController::RemoveInputMappingContext(UInputMappingContext* contextToRemove)
{
	if (!contextToRemove) return;

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->RemoveMappingContext(contextToRemove);
	}
}

void AMyPlayerController::CallMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (possessedPawn->GetClass()->ImplementsInterface(UIPlayerInput::StaticClass()))
	{
		IIPlayerInput::Execute_Move(possessedPawn, MovementVector);
	}
}

void AMyPlayerController::CallClimbMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (possessedPawn->GetClass()->ImplementsInterface(UIPlayerInput::StaticClass()))
	{
		IIPlayerInput::Execute_ClimbMove(possessedPawn, MovementVector);
	}
}

void AMyPlayerController::CallJump()
{
	if (possessedPawn->GetClass()->ImplementsInterface(UIPlayerInput::StaticClass()))
	{
		IIPlayerInput::Execute_Jump(possessedPawn);
	}
}

void AMyPlayerController::CallStopJump()
{
	if (possessedPawn->GetClass()->ImplementsInterface(UIPlayerInput::StaticClass()))
	{
		IIPlayerInput::Execute_StopJump(possessedPawn);
	}
}

void AMyPlayerController::CallLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (possessedPawn->GetClass()->ImplementsInterface(UIPlayerInput::StaticClass()))
	{
		IIPlayerInput::Execute_Look(possessedPawn, LookAxisVector);
	}
}

void AMyPlayerController::CallSprint()
{
	if (possessedPawn->GetClass()->ImplementsInterface(UIPlayerInput::StaticClass()))
	{
		IIPlayerInput::Execute_Sprint(possessedPawn);
	}
}

void AMyPlayerController::CallSwitchWalkRun()
{
	if (possessedPawn->GetClass()->ImplementsInterface(UIPlayerInput::StaticClass()))
	{
		IIPlayerInput::Execute_SwitchWalkRun(possessedPawn);
	}
}

void AMyPlayerController::CallStopSprint()
{
	if (possessedPawn->GetClass()->ImplementsInterface(UIPlayerInput::StaticClass()))
	{
		IIPlayerInput::Execute_StopSprint(possessedPawn);
	}
}

void AMyPlayerController::CallStartCharge()
{
	if (possessedPawn->GetClass()->ImplementsInterface(UIPlayerInput::StaticClass()))
	{
		chargeTimeline.PlayFromStart();
		UE_LOG(LogTemp, Warning, TEXT("Start charge"));
		IIPlayerInput::Execute_StartChargeWeapon(possessedPawn);
	}
}

void AMyPlayerController::CallStopCharge(const FInputActionInstance& Instance)
{
	if (possessedPawn->GetClass()->ImplementsInterface(UIPlayerInput::StaticClass()))
	{
		chargeTimeline.Stop();
		IIPlayerInput::Execute_ThrowWeapon(possessedPawn, chargeValue);
		UE_LOG(LogTemp, Warning, TEXT("Stop charge %f"), Instance.GetElapsedTime());
	}
}

void AMyPlayerController::CallCharging(const FInputActionInstance& Instance)
{
	if (possessedPawn->GetClass()->ImplementsInterface(UIPlayerInput::StaticClass()))
	{
		chargeTimeline.TickTimeline(GetWorld()->DeltaTimeSeconds);
	}
}

void AMyPlayerController::CallStartAiming()
{
	chargeValue = 0;
	if (possessedPawn->GetClass()->ImplementsInterface(UIPlayerInput::StaticClass()))
	{
		IIPlayerInput::Execute_StartAim(possessedPawn);
	}
}

void AMyPlayerController::CallZoomAim(const FInputActionInstance& Instance)
{
	if (possessedPawn->GetClass()->ImplementsInterface(UIPlayerInput::StaticClass()))
	{
		IIPlayerInput::Execute_OngoingAim(possessedPawn);
	}
}

void AMyPlayerController::CallStopAim(const FInputActionInstance& Instance)
{
	if (possessedPawn->GetClass()->ImplementsInterface(UIPlayerInput::StaticClass()))
	{
		IIPlayerInput::Execute_StopAim(possessedPawn);
	}
}

void AMyPlayerController::HoldCharge(float value)
{
	UE_LOG(LogTemp, Warning, TEXT("charge Timeline value: %f"), value);
	chargeValue = value;
}

void AMyPlayerController::CallInteraction()
{
	if (possessedPawn->GetClass()->ImplementsInterface(UIPlayerInput::StaticClass()))
	{
		IIPlayerInput::Execute_Interact(possessedPawn);
	}
}

void AMyPlayerController::CallStartClimb()
{
	if (possessedPawn->GetClass()->ImplementsInterface(UIPlayerInput::StaticClass()))
	{
		IIPlayerInput::Execute_StartClimbing(possessedPawn);
	}
}

void AMyPlayerController::PauseGame()
{
	UE_LOG(LogTemp, Warning, TEXT("PAUSE BUTTON"));
	if (!pauseWidgetInstance->IsVisible())
	{
		UGameplayStatics::SetGamePaused(this, true);
		ESlateVisibility visibility;
		visibility = ESlateVisibility::Visible;
		pauseWidgetInstance->SetVisibility(visibility);
		FInputModeGameAndUI mode;
		mode.SetWidgetToFocus(pauseWidgetInstance->GetCachedWidget());
		SetInputMode(mode);
		bShowMouseCursor = true;
		IIPlayerInput::Execute_Pause(possessedPawn);
	}
	else
	{
		UGameplayStatics::SetGamePaused(this, false);
		ESlateVisibility visibility;
		visibility = ESlateVisibility::Hidden;
		pauseWidgetInstance->SetVisibility(visibility);
		FInputModeGameOnly mode;
		SetInputMode(mode);
		bShowMouseCursor = false;
		IIPlayerInput::Execute_Pause(possessedPawn);
	}
}

void AMyPlayerController::CallFirstAbility()
{
	if (possessedPawn->GetClass()->ImplementsInterface(UIPlayerInput::StaticClass()))
	{
		IIPlayerInput::Execute_FirstAbility(possessedPawn);
	}
}

void AMyPlayerController::CallSecondAbility()
{
	if (possessedPawn->GetClass()->ImplementsInterface(UIPlayerInput::StaticClass()))
	{
		IIPlayerInput::Execute_SecondAbility(possessedPawn);
	}
}

void AMyPlayerController::StopClimbing()
{
	RemoveInputMappingContext(ClimbingMappingContext);
}

void AMyPlayerController::StartClimbing()
{
	AddInputMappingContext(ClimbingMappingContext, 1);
}

void AMyPlayerController::ChangePawn()
{
	possessedPawn = GetPawn();
}