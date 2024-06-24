// Copyright by Karolina Serpatowska All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/SphereComponent.h"
#include "ThrowingComponent.generated.h"

class USceneComponent;
class UProjectileMovementComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLandedDelegate);

UENUM(BlueprintType)
enum class EThrowState
{
	IDLE UMETA(DisplayName = "Idle state"),
	LANDED UMETA(DisplayName = "Landed state")
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYTHOLOGYCPP_API UThrowingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UThrowingComponent();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UProjectileMovementComponent* projectileMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* lodgePointSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* pivotSceneComponent;

	FRotator initialRotation;
	FTimeline rotationTimeline;

	FTimeline gravityTimeline;

	float chargeValue;
	FVector throwDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		EThrowState throwState = EThrowState::IDLE;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
		void CalculateThrowDirection(FVector& cameraWorldLoc, FVector& cameraForwardVec, FVector& socketLoc);

	UFUNCTION(BlueprintCallable)
		void DealWithHitAfterThrow();

	UFUNCTION(BlueprintCallable)
		void LodgeWeapon(FHitResult hit);

	UFUNCTION(BlueprintCallable)
		void RotateWeapon(float value);

	UFUNCTION(BlueprintCallable)
		FVector AdjustImpactLocation(FVector impactNormal, FVector impactLocation);

	UFUNCTION(BlueprintCallable)
		float AdjustImpactPitch(FVector impactNormal, float inclinedSurfaceRange, float regularSurfaceRange);

	UFUNCTION(BlueprintCallable)
		void TraceForCollisionDuringThrow();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwing")
		float throwTraceDistance = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwing")
		bool shouldRotate = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwing")
		float rotationSpeed = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwing")
		UCurveFloat* rotationCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwing")
		UCurveFloat* gravityCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwing")
		float gravityChangeSpeed = 4.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwing")
		TEnumAsByte<ECollisionChannel> traceChannelThrow = ECC_WorldDynamic; //channel which linetrace is IN, not which it hits, set in objects block <- this channel 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwing")
		float minChargeStrenght = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwing")
		float maxChargeStrenght = 5000.f;

	UFUNCTION(BlueprintCallable)
		void ThrowWeapon(FVector cameraWorldLoc, FVector cameraForwardVec, FVector socketLoc);

	UFUNCTION(BlueprintCallable)
		void SetGravity(float value);

	void SetLodgePoint(USceneComponent* lodgePoint);

	void SetPivotPoint(USceneComponent* pivotPoint);

	void SetInitialRotation(FRotator rotation);

	void SetChargeValue(float value);

	UFUNCTION(BlueprintCallable)
		bool CanThrow();

	UFUNCTION(BlueprintCallable)
		bool IsOnGround();

	UFUNCTION(BlueprintCallable)
		void ResetWeaponThrow();

	UPROPERTY(BlueprintAssignable)
		FLandedDelegate OnLanded;
};
