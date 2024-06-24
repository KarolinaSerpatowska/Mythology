// Copyright by Karolina Serpatowska All rights reserved


#include "Weapon.h"
#include "Components/CapsuleComponent.h"

#include "../DebugHelper.h"

// Sets default values
AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	rootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(rootSceneComponent);
	pivotSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	lodgePointSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("LodgePoint"));
	weaponBaseMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BaseMesh"));
	landedCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LandedColiision"));
	pivotSceneComponent->SetupAttachment(rootSceneComponent);
	lodgePointSceneComponent->SetupAttachment(pivotSceneComponent);
	weaponBaseMesh->SetupAttachment(lodgePointSceneComponent);
	landedCollision->SetupAttachment(lodgePointSceneComponent);
	throwingComponent = CreateDefaultSubobject<UThrowingComponent>(TEXT("ThrowingComponent"));

	pickupTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("PickupTrigger"));
	pickupTrigger->InitSphereRadius(83.f);
	pickupTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//pickupTrigger->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::PickupTriggerBeginOverlap);
	//pickupTrigger->OnComponentEndOverlap.AddDynamic(this, &AWeapon::PickupTriggerEndOverlap);
	pickupTrigger->SetupAttachment(rootSceneComponent);

	throwingComponent->SetLodgePoint(lodgePointSceneComponent);
	throwingComponent->SetPivotPoint(pivotSceneComponent);

	landedCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	if (throwingComponent) throwingComponent->OnLanded.AddDynamic(this, &AWeapon::OnLandedWeapon);
}

void AWeapon::ThrowWeapon(float chargeValue, FVector cameraWorldLoc, FVector cameraForwardVec, FVector socketLoc)
{
	throwingComponent->SetChargeValue(chargeValue);
	throwingComponent->ThrowWeapon(cameraWorldLoc, cameraForwardVec, socketLoc);
}

void AWeapon::OnLandedWeapon()
{
	pickupTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	landedCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	landedCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

}

bool AWeapon::CanThrow()
{
	return throwingComponent->CanThrow();
}

//void AWeapon::PickupTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	//canPickup = true;
//	//OnPickupWeapon.Broadcast();
//}

//void AWeapon::PickupTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//	//canPickup = false;
//}

void AWeapon::ResetWeaponState()
{
	if (throwingComponent) throwingComponent->ResetWeaponThrow();
	isOutOfBoundary = false;
	landedCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//canPickup = false;
}

//bool AWeapon::CanPickupWeapon()
//{
//	return canPickup;
//}

void AWeapon::OnInteraction_Implementation(ACharacter* player)
{
	OnPickupWeapon.Broadcast();
}

bool AWeapon::CanTeleportToWeapon()
{
	if (throwingComponent->IsOnGround())
	{
		
		return true;
	}

	return false;
}

bool AWeapon::GetOutOfBoundary()
{
	return isOutOfBoundary;
}

void AWeapon::SetOutOfBoundary(bool isOut)
{
	isOutOfBoundary = isOut;
}

void AWeapon::RecallWeapon()
{
	OnRecallWeapon.Broadcast();
}