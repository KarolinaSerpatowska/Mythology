// Copyright by Karolina Serpatowska All rights reserved


#include "ThrowingComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "CollisionQueryParams.h"
#include "Kismet/KismetMathLibrary.h"

#include "../DebugHelper.h"


// Sets default values for this component's properties
UThrowingComponent::UThrowingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetComponentTickEnabled(false);

	projectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));

	if (projectileMovementComponent)
	{
		projectileMovementComponent->Deactivate();
		projectileMovementComponent->bAutoActivate = false;
		projectileMovementComponent->bSimulationEnabled = false;
		projectileMovementComponent->ProjectileGravityScale = 0.f;
	}

}


// Called when the game starts
void UThrowingComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);

	if (shouldRotate && rotationCurve)
	{
		FOnTimelineFloat timelineUpdateFunction;
		timelineUpdateFunction.BindUFunction(this, FName("RotateWeapon"));
		rotationTimeline.AddInterpFloat(rotationCurve, timelineUpdateFunction);
	}

	if (gravityCurve)
	{
		FOnTimelineFloat timelineGravityUpdateFunction;
		timelineGravityUpdateFunction.BindUFunction(this, FName("SetGravity"));
		gravityTimeline.AddInterpFloat(gravityCurve, timelineGravityUpdateFunction);
		gravityTimeline.SetPlayRate(gravityChangeSpeed);
		gravityTimeline.PlayFromStart();
	}

}


// Called every frame
void UThrowingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	gravityTimeline.TickTimeline(DeltaTime);
	if (shouldRotate)
	{
		rotationTimeline.TickTimeline(DeltaTime);
	}
	TraceForCollisionDuringThrow();

}

void UThrowingComponent::ThrowWeapon(FVector cameraWorldLoc, FVector cameraForwardVec, FVector socketLoc)
{
	if (projectileMovementComponent && CanThrow())
	{
		GetOwner()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		if (!shouldRotate) projectileMovementComponent->bRotationFollowsVelocity = true;
		projectileMovementComponent->Activate();
		projectileMovementComponent->bSimulationEnabled = true;
		float throwStrength = UKismetMathLibrary::Lerp(minChargeStrenght, maxChargeStrenght, chargeValue);
		CalculateThrowDirection(cameraWorldLoc, cameraForwardVec, socketLoc);
		throwDirection = UKismetMathLibrary::InverseTransformDirection(GetOwner()->GetTransform(), throwDirection);
		projectileMovementComponent->SetVelocityInLocalSpace(throwDirection * throwStrength); //direction
		if (shouldRotate)
		{
			GetOwner()->SetActorRotation(initialRotation);
			rotationTimeline.SetPlayRate(rotationSpeed);
			rotationTimeline.PlayFromStart();
		}
		SetComponentTickEnabled(true);
	}
}

void UThrowingComponent::CalculateThrowDirection(FVector& cameraWorldLoc, FVector& cameraForwardVec, FVector& socketLoc)
{
	FHitResult hit;
	AActor* owner = GetOwner();
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(owner);
	const FVector end = cameraWorldLoc + (cameraForwardVec * 60000.0f);
	GetWorld()->LineTraceSingleByChannel(hit, cameraWorldLoc, end, ECollisionChannel::ECC_Visibility, queryParams);
	FVector direction = hit.TraceEnd - socketLoc;
	direction.Normalize();
	throwDirection = direction;
}

void UThrowingComponent::TraceForCollisionDuringThrow()
{
	FHitResult hit;
	AActor* owner = GetOwner();
	const FVector start = owner->GetActorLocation();
	FVector direction = owner->GetVelocity() * throwTraceDistance;
	direction.Normalize();
	const FVector end = direction + owner->GetActorLocation()+10.f;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(owner);
	queryParams.AddIgnoredActor(owner->GetOwner());

	GetWorld()->LineTraceSingleByChannel(hit, start, end, traceChannelThrow, queryParams);
	if (hit.bBlockingHit && IsValid(hit.GetActor()))
	{
		SetComponentTickEnabled(false);
		DealWithHitAfterThrow();
		LodgeWeapon(hit);
		GetOwner()->AttachToActor(hit.GetActor(), FAttachmentTransformRules::KeepWorldTransform);
		throwState = EThrowState::LANDED;
		OnLanded.Broadcast();
	}
}

void UThrowingComponent::DealWithHitAfterThrow()
{
	projectileMovementComponent->StopMovementImmediately();
	projectileMovementComponent->bSimulationEnabled = false;
	projectileMovementComponent->Deactivate();
	if (shouldRotate)
	{
		gravityTimeline.Stop();
		rotationTimeline.Stop();
		pivotSceneComponent->SetRelativeRotation(FQuat(0.f, 0.f, 0.f, 0.f));
	}
}

void UThrowingComponent::LodgeWeapon(FHitResult hit)
{
	FRotator newRotation;
	if (shouldRotate)
	{
		GetOwner()->SetActorRotation(initialRotation);
		float newPitch = AdjustImpactPitch(hit.ImpactNormal, UKismetMathLibrary::RandomFloatInRange(-30, -55), UKismetMathLibrary::RandomFloatInRange(-5, -25));
		newRotation = FRotator(newPitch, 0.0, UKismetMathLibrary::RandomFloatInRange(-3, -3));
	}
	else
	{
		newRotation = FRotator(0.0, 0.0, UKismetMathLibrary::RandomFloatInRange(-3, -3));
	}

	lodgePointSceneComponent->SetRelativeRotation(newRotation);
	GetOwner()->SetActorLocation(AdjustImpactLocation(hit.ImpactNormal, hit.ImpactPoint));
}

FVector UThrowingComponent::AdjustImpactLocation(FVector impactNormal, FVector impactLocation)
{
	FRotator rotation = UKismetMathLibrary::MakeRotationFromAxes(impactNormal, FVector::ZeroVector, FVector::ZeroVector);
	float ZAdjustment;
	if (rotation.Pitch > 0) ZAdjustment = rotation.Pitch;
	else ZAdjustment = 0;
	ZAdjustment = 90.f - ZAdjustment;
	ZAdjustment /= 90.f;
	ZAdjustment *= 10.f;
	FVector location = impactLocation + FVector(0, 0, ZAdjustment);
	location += GetOwner()->GetActorLocation() - lodgePointSceneComponent->GetComponentLocation();

	return location;
}

void UThrowingComponent::RotateWeapon(float value)
{
	float rotationY = value * (-360.f);
	FRotator newRotation = FRotator(rotationY, 0.0, 0.0);
	pivotSceneComponent->SetRelativeRotation(newRotation);
}

float UThrowingComponent::AdjustImpactPitch(FVector impactNormal, float inclinedSurfaceRange, float regularSurfaceRange)
{
	FRotator rotation = UKismetMathLibrary::MakeRotationFromAxes(impactNormal, FVector::ZeroVector, FVector::ZeroVector);
	if (rotation.Pitch > 0) return regularSurfaceRange - rotation.Pitch;
	else return inclinedSurfaceRange - rotation.Pitch;
}

void UThrowingComponent::SetGravity(float value)
{
	if (projectileMovementComponent) projectileMovementComponent->ProjectileGravityScale = value;
}

void UThrowingComponent::SetLodgePoint(USceneComponent* lodgePoint)
{
	lodgePointSceneComponent = lodgePoint;
}

void UThrowingComponent::SetPivotPoint(USceneComponent* pivotPoint)
{
	pivotSceneComponent = pivotPoint;
}

void UThrowingComponent::SetInitialRotation(FRotator rotation)
{
	initialRotation = rotation;
}

void UThrowingComponent::SetChargeValue(float value)
{
	chargeValue = value;
}

void UThrowingComponent::ResetWeaponThrow()
{
	throwState = EThrowState::IDLE;
}

bool UThrowingComponent::CanThrow()
{
	if (throwState == EThrowState::IDLE) return true;
	else return false;
}

bool UThrowingComponent::IsOnGround()
{
	if (throwState == EThrowState::LANDED) return true;
	else return false;
}