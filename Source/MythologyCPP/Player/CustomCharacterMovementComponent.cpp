// Copyright by Karolina Serpatowska All rights reserved


#include "CustomCharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MythologyCPPCharacter.h"

#include "../DebugHelper.h"

//TEMP-----------------------------------------------
void UCustomCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}
//----------------------------------------------------

void UCustomCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningPlayerAnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();

	OwningPlayerCharacter = Cast<AMythologyCPPCharacter>(CharacterOwner);

	if (OwningPlayerAnimInstance)
	{
		OwningPlayerAnimInstance->OnMontageEnded.AddDynamic(this, &UCustomCharacterMovementComponent::OnClimbMontageEnd);
		OwningPlayerAnimInstance->OnMontageBlendingOut.AddDynamic(this, &UCustomCharacterMovementComponent::OnClimbMontageEnd);
	}
}

void UCustomCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	if (IsClimbing())
	{
		bOrientRotationToMovement = false;
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(CapsuleHalfHeightWhenClimbing);
		OnStartClimbingDelegate.ExecuteIfBound();
	}

	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == ECustomMovementMode::MOVE_Climbing)
	{
		bOrientRotationToMovement = true;
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultCapsuleHalfHeight);

		const FRotator CurrentRotation = UpdatedComponent->GetComponentRotation();
		const FRotator ProperRotation = FRotator(0.f, CurrentRotation.Yaw, 0.0f);
		UpdatedComponent->SetRelativeRotation(ProperRotation);

		StopMovementImmediately();

		OnStopClimbingDelegate.ExecuteIfBound();
	}

	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

void UCustomCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	if (IsClimbing())
	{
		PhysClimbing(deltaTime, Iterations);
	}

	Super::PhysCustom(deltaTime, Iterations);
}

void UCustomCharacterMovementComponent::PhysClimbing(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	TraceForClimbableSurface();
	ProcessClimableSurfaceInfo();

	if (CheckIfShouldStopClimbing() || CheckIfReachedFloor())
	{
		StopClimbing();
	}

	RestorePreAdditiveRootMotionVelocity();

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		CalcVelocity(deltaTime, 0.f, true, MaxClimbBreakDeceleration);
	}

	ApplyRootMotionToVelocity(deltaTime);

	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FVector Adjusted = Velocity * deltaTime;
	FHitResult Hit(1.f);
	SafeMoveUpdatedComponent(Adjusted, GetClimbingRotation(deltaTime), true, Hit);

	if (Hit.Time < 1.f)
	{
		HandleImpact(Hit, deltaTime, Adjusted);
		SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}

	SnapToClimbableSurface(deltaTime);

	if (CheckIfReachedLedge())
	{
		PlayClimbingMontage(ClimbLedgeMontage);
	}
}

FVector UCustomCharacterMovementComponent::ConstrainAnimRootMotionVelocity(const FVector& RootMotionVelocity, const FVector& CurrentVelocity) const
{
	if (IsFalling() && OwningPlayerAnimInstance && OwningPlayerAnimInstance->IsAnyMontagePlaying())
	{
		return RootMotionVelocity;
	}
	else
	{
		return Super::ConstrainAnimRootMotionVelocity(RootMotionVelocity, CurrentVelocity);
	}
}

bool UCustomCharacterMovementComponent::CheckIfShouldStopClimbing()
{
	if(ClimbableSurfaceTraceResults.IsEmpty()) return true;

	const float Dot = FVector::DotProduct(CurrentClimbableSurfaceNormal, FVector::UpVector);
	const float	ThetaAngleDegrees = FMath::RadiansToDegrees(FMath::Acos(Dot));

	if (ThetaAngleDegrees <= StopClimbingAngle) return true;

	return false;
}

float UCustomCharacterMovementComponent::GetMaxSpeed() const
{
	if (IsClimbing()) return MaxClimbingSpeed;
	else return Super::GetMaxSpeed();
}

float UCustomCharacterMovementComponent::GetMaxAcceleration() const
{
	if (IsClimbing()) return MaxClimbingAcceleration;
	else return Super::GetMaxAcceleration();
}

FQuat UCustomCharacterMovementComponent::GetClimbingRotation(float DeltaTime)
{
	const FQuat CurrentCharacterRotation = UpdatedComponent->GetComponentQuat();

	if (HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity())
	{
		return CurrentCharacterRotation;
	}

	const FQuat TargetRotation = FRotationMatrix::MakeFromX(-CurrentClimbableSurfaceNormal).ToQuat();

	return FMath::QInterpTo(CurrentCharacterRotation, TargetRotation, DeltaTime, 5.f);
}

void UCustomCharacterMovementComponent::SnapToClimbableSurface(float DeltaTime)
{
	const FVector CurrentComponentForwardVec = UpdatedComponent->GetForwardVector();
	const FVector CurrentComponentLocation = UpdatedComponent->GetComponentLocation();

	const FVector ProjectedVectorToSurface = (CurrentClimbableSurfaceLocation - CurrentComponentLocation).ProjectOnTo(CurrentComponentForwardVec);

	const FVector SnapVector = -CurrentClimbableSurfaceNormal * ProjectedVectorToSurface.Length();

	UpdatedComponent->MoveComponent(SnapVector * DeltaTime * MaxClimbingSpeed, UpdatedComponent->GetComponentQuat(), true);
}

void UCustomCharacterMovementComponent::ProcessClimableSurfaceInfo()
{
	CurrentClimbableSurfaceLocation = FVector::ZeroVector;
	CurrentClimbableSurfaceNormal = FVector::ZeroVector;

	if (ClimbableSurfaceTraceResults.IsEmpty()) return;

	for (const FHitResult& HitResult : ClimbableSurfaceTraceResults)
	{
		CurrentClimbableSurfaceLocation += HitResult.ImpactPoint;
		CurrentClimbableSurfaceNormal += HitResult.ImpactNormal;
	}

	CurrentClimbableSurfaceLocation /= ClimbableSurfaceTraceResults.Num();
	CurrentClimbableSurfaceNormal = CurrentClimbableSurfaceNormal.GetSafeNormal();
}

void UCustomCharacterMovementComponent::ToogleClimbing(bool EnableClimbing)
{
	if (EnableClimbing)
	{
		if (CanStartClimbing())
		{
			PlayClimbingMontage(IdleToClimbMontage);
			
		}
		else if (CanClimbDownLedge())
		{
			PlayClimbingMontage(ClimbDownLedgeMontage);
		}
	}
	else
	{
		StopClimbing();
	}
}

void UCustomCharacterMovementComponent::PlayClimbingMontage(UAnimMontage* MontageToPlay)
{
	if (MontageToPlay && OwningPlayerAnimInstance && !OwningPlayerAnimInstance->IsAnyMontagePlaying())
	{
		OwningPlayerAnimInstance->Montage_Play(MontageToPlay);
	}
}

void UCustomCharacterMovementComponent::OnClimbMontageEnd(UAnimMontage* Montage, bool isInterrupted)
{
	if (Montage == IdleToClimbMontage || Montage==ClimbDownLedgeMontage)
	{
		StartClimbing();
		StopMovementImmediately();
	}
	
	if(Montage == ClimbLedgeMontage)
	{
		SetMovementMode(MOVE_Walking);
	}
}

bool UCustomCharacterMovementComponent::CanStartClimbing()
{
	if (IsFalling()) return false;
	if (!TraceForClimbableSurface()) return false;
	if (!LineTraceFromEyes(LineTraceDistanceforDetectingClimableSurface,0).bBlockingHit) return false;

	return true;
}

void UCustomCharacterMovementComponent::StartClimbing()
{
	SetMovementMode(MOVE_Custom, ECustomMovementMode::MOVE_Climbing);
}

void UCustomCharacterMovementComponent::StopClimbing()
{
	SetMovementMode(MOVE_Falling);
}

bool UCustomCharacterMovementComponent::IsClimbing() const
{
	if (MovementMode == MOVE_Custom && CustomMovementMode == ECustomMovementMode::MOVE_Climbing) return true;
	else return false;
}

TArray<FHitResult> UCustomCharacterMovementComponent::DoCapsuleTraceMultiByObject(const FVector& Start, const FVector& End, bool DrawDebugLines, bool DrawPersitentShape)
{
	TArray<FHitResult> CapsuleTraceHitResults;

	EDrawDebugTrace::Type DebugTraceType = EDrawDebugTrace::None;

	if (DrawPersitentShape)
	{
		DebugTraceType = EDrawDebugTrace::ForOneFrame;
		if (DrawPersitentShape)
		{
			DebugTraceType = EDrawDebugTrace::Persistent;
		}
	}

	UKismetSystemLibrary::CapsuleTraceMultiForObjects(
		this,
		Start,
		End,
		ClimbingCapsuleTraceRadius,
		ClimbingCapsuleHalfHeight,
		ClimableSurfacesTraceTypes,
		false,
		TArray<AActor*>(),
		DebugTraceType,
		CapsuleTraceHitResults,
		false
	);

	return CapsuleTraceHitResults;
}

bool UCustomCharacterMovementComponent::CheckIfReachedFloor()
{
	const FVector VectorDown = -UpdatedComponent->GetUpVector();
	const FVector StartOffset = VectorDown * 50.f;

	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + VectorDown;

	TArray<FHitResult> FloorHits = DoCapsuleTraceMultiByObject(Start, End, true, false);

	if (FloorHits.IsEmpty()) return false;

	for (const FHitResult& FloorHit : FloorHits)
	{
		const bool isFloorReached = FVector::Parallel(-FloorHit.ImpactNormal, FVector::UpVector) && GetClimbingVelocity().Z < -10.f;

		if (isFloorReached) return true;
	}

	return false;
}

bool UCustomCharacterMovementComponent::CheckIfReachedLedge()
{
	FHitResult LedgeTrace = LineTraceFromEyes(100.f, 50.f);
	if (!LedgeTrace.bBlockingHit)
	{
		const FVector Start = LedgeTrace.TraceEnd;
		FHitResult WalkableSurfaceTrace = DoLineTraceByObject(Start, Start+(-UpdatedComponent->GetUpVector() * 100.f), true, false);
		if (WalkableSurfaceTrace.bBlockingHit && GetClimbingVelocity().Z > 10.f) return true;
	}

	return false;
}

bool UCustomCharacterMovementComponent::CanClimbDownLedge()
{
	if (IsFalling()) return false;

	const FVector VectorDown = -UpdatedComponent->GetUpVector();

	const FVector FloorSurfaceTraceStart = UpdatedComponent->GetComponentLocation() + UpdatedComponent->GetForwardVector() * ClimbDownTraceOffset;
	const FVector FloorSurfaceTraceEnd = FloorSurfaceTraceStart + VectorDown * 100.f;

	FHitResult FloorHit = DoLineTraceByObject(FloorSurfaceTraceStart, FloorSurfaceTraceEnd, true, true);

	const FVector LedgeTraceStart = FloorHit.TraceStart + UpdatedComponent->GetForwardVector() * ClimbDownLedgeTraceOffset;
	const FVector LedgeTraceEnd=LedgeTraceStart + VectorDown * 200.f;

	FHitResult LedgeTraceHit = DoLineTraceByObject(LedgeTraceStart, LedgeTraceEnd, true, true);

	if (FloorHit.bBlockingHit && !LedgeTraceHit.bBlockingHit) return true;
	
	return false;
}

FHitResult UCustomCharacterMovementComponent::DoLineTraceByObject(const FVector& Start, const FVector& End, bool DrawDebugLines, bool DrawPersitentShape)
{
	FHitResult LineTraceHitResult;

	EDrawDebugTrace::Type DebugTraceType = EDrawDebugTrace::None;

	if (DrawPersitentShape)
	{
		DebugTraceType = EDrawDebugTrace::ForOneFrame;
		if (DrawPersitentShape)
		{
			DebugTraceType = EDrawDebugTrace::Persistent;
		}
	}

	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		Start,
		End,
		ClimableSurfacesTraceTypes,
		false,
		TArray<AActor*>(),
		DebugTraceType,
		LineTraceHitResult,
		false
	);

	return LineTraceHitResult;
}

//true if there are proper surface for climbing
bool UCustomCharacterMovementComponent::TraceForClimbableSurface()
{
	const FVector StartOffset = UpdatedComponent->GetForwardVector() * TraceOffsetForDetectingClimableSurface;
	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = UpdatedComponent->GetForwardVector() + Start;

	ClimbableSurfaceTraceResults = DoCapsuleTraceMultiByObject(Start, End, false, false);

	return !ClimbableSurfaceTraceResults.IsEmpty();
}

FHitResult UCustomCharacterMovementComponent::LineTraceFromEyes(float TraceDistance, float TraceStartOffset)
{
	const FVector EyeHeightOffset = UpdatedComponent->GetUpVector() * (CharacterOwner->BaseEyeHeight + TraceStartOffset);

	const FVector Start = UpdatedComponent->GetComponentLocation() + EyeHeightOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector() * TraceDistance;

	return DoLineTraceByObject(Start, End, false, false);
}

FVector UCustomCharacterMovementComponent::GetClimbingVelocity() const
{
	return UKismetMathLibrary::Quat_UnrotateVector(UpdatedComponent->GetComponentQuat(), Velocity);
}