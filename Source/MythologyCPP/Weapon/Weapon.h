// Copyright by Karolina Serpatowska All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThrowingComponent.h"
#include "../IInteraction.h"
#include "Weapon.generated.h"


class USceneComponent;
class USkeletalMeshComponent;
class UCapsuleComponent;
class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPickupWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRecallWeapon);

UCLASS()
class MYTHOLOGYCPP_API AWeapon : public AActor, public IIInteraction
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AWeapon();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* rootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* pivotSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* lodgePointSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* weaponBaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCapsuleComponent* landedCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UThrowingComponent* throwingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USphereComponent* pickupTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		bool isOutOfBoundary = false;

	//UFUNCTION(BlueprintCallable)
	//	void PickupTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//UFUNCTION(BlueprintCallable)
	//	void PickupTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/*bool canThrow = true;*/

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		bool canPickup = false;


public:
	UFUNCTION(BlueprintCallable)
		void ThrowWeapon(float chargeValue, FVector cameraWorldLoc, FVector cameraForwardVec, FVector socketLoc);

	UFUNCTION(BlueprintCallable)
		bool CanThrow();

	UFUNCTION(BlueprintCallable)
		void OnLandedWeapon();

	UFUNCTION(BlueprintCallable)
		void ResetWeaponState();

	//UFUNCTION(BlueprintCallable)
	//	bool CanPickupWeapon();

	UFUNCTION(BlueprintCallable)
		bool CanTeleportToWeapon();

	UFUNCTION(BlueprintCallable)
		void RecallWeapon();

	UFUNCTION(BlueprintCallable)
		void SetOutOfBoundary(bool isOut);

	UFUNCTION(BlueprintCallable)
		bool GetOutOfBoundary();

	UPROPERTY(BlueprintAssignable)
		FPickupWeapon OnPickupWeapon;

	UPROPERTY(BlueprintAssignable)
		FRecallWeapon OnRecallWeapon;

	void OnInteraction_Implementation(ACharacter* player) override;
};
