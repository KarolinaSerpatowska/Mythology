// Copyright by Karolina Serpatowska All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "CameraManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYTHOLOGYCPP_API UCameraManager : public UActorComponent
{
	GENERATED_BODY()


	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* cameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* followCamera;

private:
	float currentCameraZoom;
	FVector2D currentCameraOffset;
	FTimeline aimCurveTimeline;

	UFUNCTION()
		void ChangeCameraZoom(float value);

public:
	// Sets default values for this component's properties
	UCameraManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim camera")
		float zoomSpeed = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim camera")
		float defaultZoomOut = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim camera")
		float defaultZoomIn = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim camera")
		float targetZoom = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim camera")
		float cameraOffsetSpeed = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim camera")
		FVector2D defaultCameraOffset {
		0.f, 0.f
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim camera")
		FVector2D targetCameraOffset {
		150.f, 70.f
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim camera")
		FVector2D defaultCameraOffsetAim {
		150.f, 70.f
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim camera")
		UCurveFloat* aimZoomCurve;

	UFUNCTION(BlueprintCallable)
		void StartAim();

	UFUNCTION(BlueprintCallable)
		void StopAim();

	UFUNCTION(BlueprintCallable)
		void OngoingAim();

	UFUNCTION(BlueprintCallable)
		void SetCamera(USpringArmComponent* springArm, UCameraComponent* camera);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


		
};
