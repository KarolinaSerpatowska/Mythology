// Copyright Epic Games, Inc. All Rights Reserved.

#include "MythologyCPPCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "CustomCharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "../IInteraction.h"
#include "../DebugHelper.h"


//////////////////////////////////////////////////////////////////////////
// AMythologyCPPCharacter

AMythologyCPPCharacter::AMythologyCPPCharacter(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	MyCustomCharacterMovementComponent = Cast<UCustomCharacterMovementComponent>(GetCharacterMovement());

	// Configure character movement
	MyCustomCharacterMovementComponent->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	MyCustomCharacterMovementComponent->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	MyCustomCharacterMovementComponent->JumpZVelocity = 700.f;
	MyCustomCharacterMovementComponent->AirControl = 0.35f;
	MyCustomCharacterMovementComponent->MaxWalkSpeed = defaultRunSpeed;
	MyCustomCharacterMovementComponent->MinAnalogWalkSpeed = 20.f;
	MyCustomCharacterMovementComponent->BrakingDecelerationWalking = 2000.f;

	interactionSphereTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionTrigger"));
	interactionSphereTrigger->OnComponentBeginOverlap.AddDynamic(this, &AMythologyCPPCharacter::SphereTriggerBeginOverlap);
	interactionSphereTrigger->OnComponentEndOverlap.AddDynamic(this, &AMythologyCPPCharacter::SphereTriggerEndOverlap);
	interactionSphereTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	interactionSphereTrigger->SetSphereRadius(110.f);
	interactionSphereTrigger->SetupAttachment(GetMesh());

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	
	cameraManager = CreateDefaultSubobject<UCameraManager>(TEXT("CameraManager"));
	cameraManager->Activate();

	abilityManager = CreateDefaultSubobject<UAbilitiesManager>(TEXT("AbilitiesManager")); //

	attributesManager = CreateDefaultSubobject<UAttributesManager>(TEXT("AttributesManager"));
}

void AMythologyCPPCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	PlayerAnimInstance = GetMesh()->GetAnimInstance();

	if (CameraBoom && FollowCamera && cameraManager)
	{
		cameraManager->SetCamera(CameraBoom, FollowCamera);
	}

	playerController = Cast<AMyPlayerController>(GetController());

	if (MyCustomCharacterMovementComponent)
	{
		MyCustomCharacterMovementComponent->OnStartClimbingDelegate.BindUObject(this, &ThisClass::OnPlayerStartClimbing);
		MyCustomCharacterMovementComponent->OnStopClimbingDelegate.BindUObject(this, &ThisClass::OnPlayerStopClimbing);
	}

	if (attributesManager)
	{
		attributesManager->OnNoStaminaDelegate.BindUObject(this, &ThisClass::StartClimb);
	}

	if (GetWorld())
	{
		FVector location = GetActorLocation();
		location.Z += 90.f;
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, location, FRotator::ZeroRotator, SpawnParams);
		weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("spearHandSocket"));
		weapon->OnPickupWeapon.AddDynamic(this, &AMythologyCPPCharacter::OnWeaponPickup);
		weapon->OnRecallWeapon.AddDynamic(this, &AMythologyCPPCharacter::AttachWeaponAgain);
	}

	if (abilityManager)
	{
		abilityManager->InitAbilities();
	}

}

USkeletalMeshComponent* AMythologyCPPCharacter::GetCharacterMesh_Implementation()
{
	return GetMesh();
}

AWeapon* AMythologyCPPCharacter::GetWeapon_Implementation()
{
	return weapon;
}

bool AMythologyCPPCharacter::CanUseAbilities_Implementation()
{
	if (MyCustomCharacterMovementComponent->IsClimbing()) return false;
	return true;
}

void AMythologyCPPCharacter::Move_Implementation(const FVector2D& Value)
{
	if (!PlayerAnimInstance->Montage_IsPlaying(landingMontage))
	{
		GroundMovementInput(Value);
	}
}

void AMythologyCPPCharacter::ClimbMove_Implementation(const FVector2D& Value)
{
	if (!PlayerAnimInstance->Montage_IsPlaying(landingMontage))
	{
		ClimbingMovementInput(Value);
	}
}

void AMythologyCPPCharacter::GroundMovementInput(const FVector2D& Value)
{
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Value.Y);
		AddMovementInput(RightDirection, Value.X);
	}
}

void AMythologyCPPCharacter::ClimbingMovementInput(const FVector2D& Value)
{
	const FVector ForwardDirection = FVector::CrossProduct(-MyCustomCharacterMovementComponent->GetCurrentClimbableSurfaceNormal(), GetActorRightVector());

	const FVector RightDirection = FVector::CrossProduct(-MyCustomCharacterMovementComponent->GetCurrentClimbableSurfaceNormal(), -GetActorUpVector());

	// add movement 
	AddMovementInput(ForwardDirection, Value.Y);
	AddMovementInput(RightDirection, Value.X);
}

void AMythologyCPPCharacter::StartClimbing_Implementation()
{
	if (!isAiming && !PlayerAnimInstance->Montage_IsPlaying(landingMontage))
	{
		StartClimb();
	}
}

void AMythologyCPPCharacter::Look_Implementation(const FVector2D& Value)
{
	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Value.X);
		AddControllerPitchInput(Value.Y);
	}
}

void AMythologyCPPCharacter::Jump_Implementation()
{
	if (Controller != nullptr && !MyCustomCharacterMovementComponent->IsClimbing() && !PlayerAnimInstance->Montage_IsPlaying(landingMontage))
	{
		ACharacter::Jump();
	}
}

void AMythologyCPPCharacter::StopJump_Implementation()
{
	if (Controller != nullptr)
	{
		ACharacter::StopJumping();
	}
}

void AMythologyCPPCharacter::Sprint_Implementation()
{
	if (!MyCustomCharacterMovementComponent->IsClimbing() && !PlayerAnimInstance->Montage_IsPlaying(landingMontage))
	{
		UE_LOG(LogTemp, Warning, TEXT("SPRINT!"));
		GetCharacterMovement()->MaxWalkSpeed = defaultSprintSpeed;
		//isSprinting = true;
		isWalking = false;
	}
}

void AMythologyCPPCharacter::StopSprint_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("NOT SPRINT"));
	GetCharacterMovement()->MaxWalkSpeed = defaultRunSpeed;
	//isSprinting = false;
	isWalking = false;
}

void AMythologyCPPCharacter::SwitchWalkRun_Implementation()
{
	if (!MyCustomCharacterMovementComponent->IsClimbing() && !PlayerAnimInstance->Montage_IsPlaying(landingMontage))
	{
		UE_LOG(LogTemp, Warning, TEXT("WALK BUTTON!"));
		if (isWalking)
		{
			SwitchWalkToRun();
		}
		else if (!isWalking)
		{
			SwitchRunToWalk();
		}
	}
}

void AMythologyCPPCharacter::StartAim_Implementation()
{
	if (!MyCustomCharacterMovementComponent->IsClimbing() && !PlayerAnimInstance->Montage_IsPlaying(landingMontage))
	{
		cameraManager->StartAim();
		bUseControllerRotationYaw = true;
		isAiming = true;
		OnIsAimingChanged.Broadcast();
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}

}

void AMythologyCPPCharacter::OngoingAim_Implementation()
{
	if (!MyCustomCharacterMovementComponent->IsClimbing() && !PlayerAnimInstance->Montage_IsPlaying(landingMontage))
	{
		cameraManager->OngoingAim();
	}
}

void AMythologyCPPCharacter::StopAim_Implementation()
{
	if (!PlayerAnimInstance->Montage_IsPlaying(throwMontage) && isAiming)
	{
		cameraManager->StopAim();
		bUseControllerRotationYaw = false;
		isAiming = false;
		OnIsAimingChanged.Broadcast();
		GetCharacterMovement()->bOrientRotationToMovement = true;
		if (PlayerAnimInstance->Montage_IsPlaying(chargeThrowMontage))
		{
			PlayerAnimInstance->Montage_Stop(0.3f, chargeThrowMontage);
		}
	}
}

void AMythologyCPPCharacter::StartChargeWeapon_Implementation()
{
	if (!MyCustomCharacterMovementComponent->IsClimbing() && !PlayerAnimInstance->Montage_IsPlaying(landingMontage))
	{
		chargeValue = 0;
		if (weapon && isAiming && weapon->CanThrow())
		{
			PlaySomeMontage(chargeThrowMontage);
		}
	}

}

void AMythologyCPPCharacter::ThrowWeapon_Implementation(float value)
{
	if (weapon && isAiming && weapon->CanThrow() && !MyCustomCharacterMovementComponent->IsClimbing() && !PlayerAnimInstance->Montage_IsPlaying(landingMontage))
	{
		weapon->ResetWeaponState();
		PlaySomeMontage(throwMontage);
		chargeValue = value;
	}
}

void AMythologyCPPCharacter::Interact_Implementation()
{
	//call on interaction - on first object or the closest

	if (!interactables.IsEmpty())
	{
		if (interactables[0]->GetClass()->ImplementsInterface(UIInteraction::StaticClass()))
		{
			IIInteraction* interaction = Cast<IIInteraction>(interactables[0]);
			interaction->Execute_OnInteraction(interactables[0], this);

			if (!interactables.IsEmpty() && IsValid(Cast<AWeapon>(interactables[0]))) //object is weapon
			{
				interactables.RemoveAt(0); //remove pickuped weapon
			}
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

	/*if (weapon->CanPickupWeapon())
	{
		AttachWeaponAgain();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("INTERACTIOn without pickup"));
	}*/
}

void AMythologyCPPCharacter::FirstAbility_Implementation()
{
	if (abilityManager && !PlayerAnimInstance->Montage_IsPlaying(landingMontage)) {
		UE_LOG(LogTemp, Warning, TEXT("INPUT 0"));
		abilityManager->UseAbility(0);
	}
}

void AMythologyCPPCharacter::SecondAbility_Implementation()
{
	/*if (abilityManager && !PlayerAnimInstance->Montage_IsPlaying(landingMontage)) {
		UE_LOG(LogTemp, Warning, TEXT("INPUT 1"));
		abilityManager->UseAbility(1);
	}*/
	if(abilityManager && !PlayerAnimInstance->Montage_IsPlaying(landingMontage) && !weapon)
	{
		if (GetWorld())
		{
			UE_LOG(LogTemp, Warning, TEXT("spawn 1 if"));
			FVector location = GetActorLocation();
			location.Z += 90.f;
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();
			weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, location, FRotator::ZeroRotator, SpawnParams);
			weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("spearHandSocket"));
			weapon->OnPickupWeapon.AddDynamic(this, &AMythologyCPPCharacter::OnWeaponPickup);
			weapon->OnRecallWeapon.AddDynamic(this, &AMythologyCPPCharacter::AttachWeaponAgain);
		}
	}
	else if (abilityManager && !PlayerAnimInstance->Montage_IsPlaying(landingMontage) && (weapon->GetDistanceTo(this) >= recallDistance))
	{
		UE_LOG(LogTemp, Warning, TEXT("destroy"));
		weapon->Destroy();
		if (GetWorld())
		{
			UE_LOG(LogTemp, Warning, TEXT("spawn"));
			FVector location = GetActorLocation();
			location.Z += 90.f;
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();
			weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, location, FRotator::ZeroRotator, SpawnParams);
			weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("spearHandSocket"));
			weapon->OnPickupWeapon.AddDynamic(this, &AMythologyCPPCharacter::OnWeaponPickup);
			weapon->OnRecallWeapon.AddDynamic(this, &AMythologyCPPCharacter::AttachWeaponAgain);
		}
	}
	else if (abilityManager && !PlayerAnimInstance->Montage_IsPlaying(landingMontage)) {
		UE_LOG(LogTemp, Warning, TEXT("INPUT 1"));
		abilityManager->UseAbility(1);
	}

	
}

void AMythologyCPPCharacter::Pause_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("PAUSE"));
}

void AMythologyCPPCharacter::AttachWeaponAgain()
{
	if (weapon)
	{
		weapon->ResetWeaponState();
		weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("spearHandSocket"));
	}
}

void AMythologyCPPCharacter::OnThrowWeaponAfterMontageEnded()
{
	if (weapon && isAiming && weapon->CanThrow())
	{
		UE_LOG(LogTemp, Warning, TEXT("FIRE"));
		weapon->ThrowWeapon(chargeValue, FollowCamera->GetComponentLocation(), FollowCamera->GetForwardVector(), GetMesh()->GetSocketLocation(FName("spearHandSocket")));
		StopAim_Implementation();
	}
}

void AMythologyCPPCharacter::SwitchWalkToRun()
{
	GetCharacterMovement()->MaxWalkSpeed = defaultRunSpeed;
	isWalking = false;
}

void AMythologyCPPCharacter::SwitchRunToWalk()
{
	GetCharacterMovement()->MaxWalkSpeed = defaultWalkSpeed;
	isWalking = true;
}

void AMythologyCPPCharacter::OnWeaponPickup()
{
	AttachWeaponAgain();
}


void AMythologyCPPCharacter::PlaySomeMontage(UAnimMontage* MontageToPlay)
{
	if (MontageToPlay && PlayerAnimInstance && (MontageToPlay == chargeThrowMontage || MontageToPlay == throwMontage))
	{
		PlayerAnimInstance->Montage_Play(MontageToPlay);
	}
	else if (MontageToPlay && PlayerAnimInstance && !PlayerAnimInstance->IsAnyMontagePlaying())
	{
		PlayerAnimInstance->Montage_Play(MontageToPlay);
	}
}

void AMythologyCPPCharacter::StartClimb()
{
	if (!MyCustomCharacterMovementComponent) return;

	if (!MyCustomCharacterMovementComponent->IsClimbing())
	{
		MyCustomCharacterMovementComponent->ToogleClimbing(true);
	}
	else
	{
		MyCustomCharacterMovementComponent->ToogleClimbing(false);
	}
}

void AMythologyCPPCharacter::OnPlayerStartClimbing()
{
	if (playerController) playerController->OnStartClimbingDelegate.ExecuteIfBound();
	attributesManager->DecreaseStamina();
}

void AMythologyCPPCharacter::OnPlayerStopClimbing()
{
	if (playerController) playerController->OnStopClimbingDelegate.ExecuteIfBound();
	attributesManager->RecoverStamina();
}

void AMythologyCPPCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	double fallingVelocity = abs(GetVelocity().Z);
	if (fallingVelocity >= 1400.f)
	{
		PlayerAnimInstance->Montage_Play(landingMontage);
		UE_LOG(LogTemp, Warning, TEXT("fall velocity: %f"), fallingVelocity);
		
	}
}

void AMythologyCPPCharacter::SphereTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void AMythologyCPPCharacter::SphereTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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