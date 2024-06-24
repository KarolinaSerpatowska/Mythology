// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "IPlayerInput.h"
#include "../IPawnsUtilities.h"
#include "MyPlayerController.h"
#include "CameraManager.h"
#include "../Weapon/Weapon.h"
#include "../Abilities/AbilitiesManager.h"
#include "AttributesManager.h"
#include "MythologyCPPCharacter.generated.h"

class UCustomCharacterMovementComponent;
class USpringArmComponent;
class UCameraComponent;
class UAnimMontage;
class UAnimInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FIsAimingDelegate);

UCLASS(config=Game)
class AMythologyCPPCharacter : public ACharacter, public IIPlayerInput, public IIPawnsUtilities
{
	GENERATED_BODY()

public:
	AMythologyCPPCharacter(const FObjectInitializer& ObjectInitializer);

private:
	AMyPlayerController* playerController;

	UPROPERTY()
		UAnimInstance* PlayerAnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montages", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* throwMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montages", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* chargeThrowMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montages", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* landingMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USphereComponent* interactionSphereTrigger;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraManager* cameraManager;

	//Movement component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UCustomCharacterMovementComponent* MyCustomCharacterMovementComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
		AWeapon* weapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
		UAbilitiesManager* abilityManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Attributes, meta = (AllowPrivateAccess = "true"))
		UAttributesManager* attributesManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = test, meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> interactables;

	float chargeValue = 0;

	void PlaySomeMontage(UAnimMontage* MontageToPlay);

	void SwitchWalkToRun();
	void SwitchRunToWalk();

	void GroundMovementInput(const FVector2D& Value);

	void ClimbingMovementInput(const FVector2D& Value);

	void StartClimb();

	void OnPlayerStartClimbing();
	void OnPlayerStopClimbing();

	UFUNCTION(BlueprintCallable)
	void SphereTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void SphereTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	
	// To add mapping context
	virtual void BeginPlay();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		bool isWalking = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		bool isAiming = false;

	/*UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CharacterVariables, meta = (AllowPrivateAccess = "true"))
		bool isSprinting = false;*/

	void Landed(const FHitResult& Hit) override;

public:
	USkeletalMeshComponent* GetCharacterMesh_Implementation() override;

	AWeapon* GetWeapon_Implementation() override;

	bool CanUseAbilities_Implementation() override;

	/** Called for movement input */
	void Move_Implementation(const FVector2D& Value) override;
	void ClimbMove_Implementation(const FVector2D& Value) override;

	/** Called for looking input */
	void Look_Implementation(const FVector2D& Value) override;

	void Jump_Implementation() override;
	void StopJump_Implementation() override;

	void Sprint_Implementation() override;
	void StopSprint_Implementation() override;

	void SwitchWalkRun_Implementation() override;

	void StartAim_Implementation() override;
	void OngoingAim_Implementation() override;
	void StopAim_Implementation() override;

	void StartChargeWeapon_Implementation() override;
	void ThrowWeapon_Implementation(float value) override;

	void Interact_Implementation() override;

	void FirstAbility_Implementation() override;
	void SecondAbility_Implementation() override;

	void StartClimbing_Implementation() override;

	void Pause_Implementation() override;

	UFUNCTION(BlueprintCallable)
		void AttachWeaponAgain();

	UFUNCTION(BlueprintCallable)
		void OnThrowWeaponAfterMontageEnded();

	UFUNCTION(BlueprintCallable)
		void OnWeaponPickup();

	UPROPERTY(EditAnywhere, Category = Weapon)
		TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, Category = Weapon)
		float recallDistance = 10000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		float defaultRunSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		float defaultSprintSpeed = 1600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterVariables)
		float defaultWalkSpeed = 100.f;

	UFUNCTION(BlueprintCallable)
		bool GetIsAiming() const { return isAiming; }

	UPROPERTY(BlueprintAssignable)
		FIsAimingDelegate OnIsAimingChanged;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE UCustomCharacterMovementComponent* GetCustomCharacterMovementComponent() const { return MyCustomCharacterMovementComponent; }
};

