// Copyright by Karolina Serpatowska All rights reserved


#include "ProloguePlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "../IInteraction.h"

// Sets default values
AProloguePlayer::AProloguePlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	interactionSphereTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionTrigger"));
	interactionSphereTrigger->OnComponentBeginOverlap.AddDynamic(this, &AProloguePlayer::SphereTriggerBeginOverlap);
	interactionSphereTrigger->OnComponentEndOverlap.AddDynamic(this, &AProloguePlayer::SphereTriggerEndOverlap);
	interactionSphereTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	interactionSphereTrigger->SetSphereRadius(110.f);
	interactionSphereTrigger->SetupAttachment(GetMesh());

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = true;
}

// Called when the game starts or when spawned
void AProloguePlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProloguePlayer::Move_Implementation(const FVector2D& Value)
{
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, Value.Y);
		AddMovementInput(RightDirection, Value.X);
	}
}

void AProloguePlayer::Look_Implementation(const FVector2D& Value)
{
	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Value.X);
		AddControllerPitchInput(Value.Y);
	}
}

void AProloguePlayer::Interact_Implementation()
{
	if (!interactables.IsEmpty())
	{
		if (interactables[0]->GetClass()->ImplementsInterface(UIInteraction::StaticClass()))
		{
			IIInteraction* interaction = Cast<IIInteraction>(interactables[0]);
			interaction->Execute_OnInteraction(interactables[0], this);

		}
		else //interact component
		{
			TArray<UActorComponent*> interactableComponents = interactables[0]->GetComponentsByInterface(UIInteraction::StaticClass());
			for (auto interactable : interactableComponents)
			{
				IIInteraction* interaction = Cast<IIInteraction>(interactable);
				interaction->Execute_OnInteraction(interactable, this);
			}
		}

	}
}

void AProloguePlayer::SphereTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetClass()->ImplementsInterface(UIInteraction::StaticClass()))
	{
		interactables.AddUnique(OtherActor);
	}

	TArray<UActorComponent*> interactableComponents = OtherActor->GetComponentsByInterface(UIInteraction::StaticClass());
	for (auto interactable : interactableComponents)
	{
		interactables.AddUnique(interactable->GetOwner());
	}
}

void AProloguePlayer::SphereTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->GetClass()->ImplementsInterface(UIInteraction::StaticClass()))
	{
		interactables.Remove(OtherActor);
	}

	TArray<UActorComponent*> interactableComponents = OtherActor->GetComponentsByInterface(UIInteraction::StaticClass());
	for (auto interactable : interactableComponents)
	{
		interactables.Remove(interactable->GetOwner());
	}
}