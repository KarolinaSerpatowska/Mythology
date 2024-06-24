// Copyright by Karolina Serpatowska All rights reserved


#include "RunnerPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ARunnerPlayer::ARunnerPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 200.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false;

	OnDieDelegate.BindUObject(this, &ARunnerPlayer::Die);

}

// Called when the game starts or when spawned
void ARunnerPlayer::BeginPlay()
{
	Super::BeginPlay();
	points = 0;
	canMove = true;

	playerController = Cast<AMyPlayerController>(GetController());

	if (HUDClass)
	{
		if (!HUDWidgetInstance)
		{
			HUDWidgetInstance = CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(), 0), HUDClass);
			if (!HUDWidgetInstance->IsInViewport())
			{
				HUDWidgetInstance->AddToViewport();
				ESlateVisibility visibility;
				visibility = ESlateVisibility::Visible;
				HUDWidgetInstance->SetVisibility(visibility);
			}
		}
	}

}

// Called every frame
void ARunnerPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveForward();
}

void ARunnerPlayer::Move_Implementation(const FVector2D& Value)
{
	if (Controller != nullptr && canMove)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(RightDirection, Value.X);
	}
}

void ARunnerPlayer::MoveForward()
{
	if (Controller != nullptr && canMove)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection, 1.0f);
	}
}

void ARunnerPlayer::Pause_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("PAUSE"));
	if (HUDWidgetInstance->GetVisibility() == ESlateVisibility::Visible)
	{
		HUDWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		HUDWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	}
}

void ARunnerPlayer::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("DIE"));
	GetCharacterMovement()->StopMovementImmediately();
	canMove = false;
	OnDieDelegate.Unbind();
	if (EndUIClass)
	{
		if (!EndUIWidgetInstance)
		{
			EndUIWidgetInstance = CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(), 0), EndUIClass);
			if (!EndUIWidgetInstance->IsInViewport())
			{
				UGameplayStatics::SetGamePaused(this, true);
				EndUIWidgetInstance->AddToViewport();
				ESlateVisibility visibility;
				visibility = ESlateVisibility::Visible;
				EndUIWidgetInstance->SetVisibility(visibility);
				HUDWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
				FInputModeUIOnly mode;
				mode.SetWidgetToFocus(EndUIWidgetInstance->GetCachedWidget());
				if (playerController)
				{
					playerController->SetInputMode(mode);
					playerController->bShowMouseCursor = true;
				}
			}
		}
	}
}

void ARunnerPlayer::GivePoints()
{
	points++;
}

int ARunnerPlayer::GetPoints()
{
	return points;
}

void ARunnerPlayer::EndRun()
{
	UE_LOG(LogTemp, Warning, TEXT("DONE"));
	if (EndUIClass)
	{
		if (!EndUIWidgetInstance)
		{
			EndUIWidgetInstance = CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(), 0), EndUIClass);
			if (!EndUIWidgetInstance->IsInViewport())
			{
				UGameplayStatics::SetGamePaused(this, true);
				EndUIWidgetInstance->AddToViewport();
				ESlateVisibility visibility;
				visibility = ESlateVisibility::Visible;
				EndUIWidgetInstance->SetVisibility(visibility);
				HUDWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
				FInputModeGameAndUI mode;
				mode.SetWidgetToFocus(EndUIWidgetInstance->GetCachedWidget());
				if (playerController)
				{
					playerController->SetInputMode(mode);
					playerController->bShowMouseCursor = true;
				}
			}
		}
	}
}