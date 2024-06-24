// Copyright by Karolina Serpatowska All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomCharacterMovementComponent.generated.h"

class UAnimMontage;
class UAnimInstance;
class AMythologyCPPCharacter;

DECLARE_DELEGATE(FOnStartClimbing)
DECLARE_DELEGATE(FOnStopClimbing)

UENUM(BlueprintType)
namespace ECustomMovementMode
{
	enum Type
	{
		MOVE_Climbing UMETA(DisplayName = "Climb Mode")
	};
}

/**
 * 
 */
UCLASS()
class MYTHOLOGYCPP_API UCustomCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxAcceleration() const override;
	virtual FVector ConstrainAnimRootMotionVelocity(const FVector& RootMotionVelocity, const FVector& CurrentVelocity) const override;

private:
	//Climbing variables-----------------------------------------------------------
	UPROPERTY()
	UAnimInstance* OwningPlayerAnimInstance;

	UPROPERTY()
	AMythologyCPPCharacter* OwningPlayerCharacter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement : Climbing", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* IdleToClimbMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement : Climbing", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ClimbLedgeMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement : Climbing", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ClimbDownLedgeMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement : Climbing", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> ClimableSurfacesTraceTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement : Climbing", meta = (AllowPrivateAccess = "true"))
	float ClimbingCapsuleTraceRadius=50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement : Climbing", meta = (AllowPrivateAccess = "true"))
	float ClimbingCapsuleHalfHeight=72.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement : Climbing", meta = (AllowPrivateAccess = "true"))
	float TraceOffsetForDetectingClimableSurface = 30.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement : Climbing", meta = (AllowPrivateAccess = "true"))
	float LineTraceDistanceforDetectingClimableSurface = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement : Climbing", meta = (AllowPrivateAccess = "true"))
	float CapsuleHalfHeightWhenClimbing = 48.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement : Climbing", meta = (AllowPrivateAccess = "true"))
	float DefaultCapsuleHalfHeight = 96.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement : Climbing", meta = (AllowPrivateAccess = "true"))
	float MaxClimbBreakDeceleration = 400.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement : Climbing", meta = (AllowPrivateAccess = "true"))
	float MaxClimbingSpeed = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement : Climbing", meta = (AllowPrivateAccess = "true"))
	float MaxClimbingAcceleration = 300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement : Climbing", meta = (AllowPrivateAccess = "true"))
	float StopClimbingAngle = 60.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement : Climbing", meta = (AllowPrivateAccess = "true"))
	float ClimbDownTraceOffset = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement : Climbing", meta = (AllowPrivateAccess = "true"))
	float ClimbDownLedgeTraceOffset = 50.f;

	FVector CurrentClimbableSurfaceLocation;

	FVector CurrentClimbableSurfaceNormal;

	TArray<FHitResult> ClimbableSurfaceTraceResults;

	//Climbing functions-----------------------------------------------------------
	TArray <FHitResult> DoCapsuleTraceMultiByObject(const FVector& Start, const FVector& End, bool DrawDebugLines, bool DrawPersitentShape);

	FHitResult DoLineTraceByObject(const FVector& Start, const FVector& End, bool DrawDebugLines, bool DrawPersitentShape);

	void PlayClimbingMontage(UAnimMontage* MontageToPlay);

	UFUNCTION()
	void OnClimbMontageEnd(UAnimMontage* Montage, bool isInterrupted);

	//Main climbing mechanics-----------------------------------------------------------
	bool TraceForClimbableSurface();

	FHitResult LineTraceFromEyes(float TraceDistance, float TraceStartOffset);

	bool CanStartClimbing();

	void StartClimbing();

	void StopClimbing();

	void PhysClimbing(float deltaTime, int32 Iterations);

	void ProcessClimableSurfaceInfo();

	FQuat GetClimbingRotation(float DeltaTime);

	void SnapToClimbableSurface(float DeltaTime);

	bool CheckIfShouldStopClimbing();

	bool CheckIfReachedFloor();

	bool CheckIfReachedLedge();

	bool CanClimbDownLedge();

public:
	FOnStartClimbing OnStartClimbingDelegate;
	FOnStopClimbing OnStopClimbingDelegate;

	void ToogleClimbing(bool EnableClimbing);

	UFUNCTION(BlueprintPure)
	bool IsClimbing() const;

	UFUNCTION(BlueprintPure)
	FVector GetClimbingVelocity() const;

	FORCEINLINE FVector GetCurrentClimbableSurfaceNormal() const { return CurrentClimbableSurfaceNormal; }
};
