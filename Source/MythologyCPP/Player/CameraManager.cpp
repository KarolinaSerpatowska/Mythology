// Copyright by Karolina Serpatowska All rights reserved


#include "CameraManager.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"


// Sets default values for this component's properties
UCameraManager::UCameraManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetComponentTickEnabled(false);

	currentCameraZoom = defaultZoomOut;
	targetZoom = defaultZoomIn;
	currentCameraOffset = defaultCameraOffset;
	targetCameraOffset = defaultCameraOffsetAim;
}

// Called when the game starts
void UCameraManager::BeginPlay()
{
	Super::BeginPlay();

	if (aimZoomCurve)
	{
		FOnTimelineFloat timelineUpdateFunction;
		timelineUpdateFunction.BindUFunction(this, FName("ChangeCameraZoom"));
		aimCurveTimeline.AddInterpFloat(aimZoomCurve, timelineUpdateFunction);
	}
}

void UCameraManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (cameraBoom && cameraBoom->TargetArmLength >= defaultZoomOut)
	{
		aimCurveTimeline.Stop();
		SetComponentTickEnabled(false);
	}
	else
	{
		aimCurveTimeline.TickTimeline(DeltaTime * zoomSpeed);
	}
}

void UCameraManager::ChangeCameraZoom(float value)
{
	if (cameraBoom)
	{
		cameraBoom->TargetArmLength = FMath::Lerp(currentCameraZoom, targetZoom, value);
		cameraBoom->SocketOffset.Y = FMath::Lerp(currentCameraOffset.X, targetCameraOffset.X, value);
		cameraBoom->SocketOffset.Z = FMath::Lerp(currentCameraOffset.Y, targetCameraOffset.Y, value);
	}
}


void UCameraManager::StartAim()
{
	targetZoom = defaultZoomIn;
	targetCameraOffset = defaultCameraOffsetAim;
	UE_LOG(LogTemp, Warning, TEXT("START AIM"));
	aimCurveTimeline.Play();
}

void UCameraManager::StopAim()
{
	UE_LOG(LogTemp, Warning, TEXT("STOP AIM"));
	aimCurveTimeline.Stop();
	aimCurveTimeline.Reverse();
	SetComponentTickEnabled(true);
}

void UCameraManager::OngoingAim()
{
	aimCurveTimeline.TickTimeline(GetWorld()->DeltaTimeSeconds * zoomSpeed);
}

void UCameraManager::SetCamera(USpringArmComponent* springArm, UCameraComponent* camera)
{
	if (springArm && camera)
	{
		cameraBoom = springArm;
		followCamera = camera;
		cameraBoom->TargetArmLength = defaultZoomOut; // The camera follows at this distance behind the character	
		cameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
		followCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	}
}
