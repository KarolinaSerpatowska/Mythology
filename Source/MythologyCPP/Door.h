// Copyright by Karolina Serpatowska All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "IInteraction.h"
#include "Door.generated.h"

UCLASS()
class MYTHOLOGYCPP_API ADoor : public AActor, public IIInteraction
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* door;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* doorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* interactionBoxTrigger;

	FTimeline doorTimeline;

	bool isClosed = true;

	UFUNCTION(BlueprintCallable)
		void BoxTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void OpenDoor(float value);

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
		UCurveFloat* doorCurve;

	void OnInteraction_Implementation(ACharacter* player) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
		float openDoorAngle = 90.f;
};
