// Copyright by Karolina Serpatowska All rights reserved


#include "PickupObject.h"
#include "Components/CapsuleComponent.h"
#include "../Player/FoodPlayer.h"

// Sets default values
APickupObject::APickupObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	pickupTrigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("PickupTrigger"));
	pickupTrigger->OnComponentBeginOverlap.AddDynamic(this, &APickupObject::PickupTriggerBeginOverlap);
	//pickupTrigger->OnComponentEndOverlap.AddDynamic(this, &APickupObject::PickupTriggerEndOverlap);

	pickupComponent = CreateDefaultSubobject<UPickupComponent>(TEXT("PickupComponent"));

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	mesh->SetupAttachment(pickupTrigger);

	pickupTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

}

// Called when the game starts or when spawned
void APickupObject::BeginPlay()
{
	Super::BeginPlay();
	
}

void APickupObject::PickupTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	pickupTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AFoodPlayer* player = Cast<AFoodPlayer>(OtherActor);
	if (player)
	{
		player->IncrementTime();
	}
	pickupComponent->DealWithPickup();
}
