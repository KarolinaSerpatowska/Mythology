// Copyright by Karolina Serpatowska All rights reserved

#include "Door.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	door = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	door->SetupAttachment(root);
	doorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshDoor"));
	doorMesh->SetupAttachment(door);
	interactionBoxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionTrigger"));
	interactionBoxTrigger->OnComponentEndOverlap.AddDynamic(this, &ADoor::BoxTriggerEndOverlap);
	interactionBoxTrigger->SetupAttachment(root);
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
	isClosed = true;

	if (doorCurve)
	{
		FOnTimelineFloat timelineUpdateFunction;
		timelineUpdateFunction.BindUFunction(this, FName("OpenDoor"));
		doorTimeline.AddInterpFloat(doorCurve, timelineUpdateFunction);
	}

}

void ADoor::Tick(float DeltaTime)
{
	doorTimeline.TickTimeline(DeltaTime);
}

void ADoor::OnInteraction_Implementation(ACharacter* player)
{
	//open/close door

	SetActorTickEnabled(true);
	
	if (isClosed) //open
	{
		doorTimeline.Play();
		isClosed = false;
	}
	else //close
	{
		doorTimeline.Reverse();
		isClosed = true;
	}
}

void ADoor::BoxTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//close door
	doorTimeline.Reverse();
	isClosed = true;
}

void ADoor::OpenDoor(float value)
{
	FRotator NewRotation = FRotator(0.f, openDoorAngle * value, 0.f);
	FQuat QuatRotation = FQuat(NewRotation);
	door->SetRelativeRotation(QuatRotation);
}