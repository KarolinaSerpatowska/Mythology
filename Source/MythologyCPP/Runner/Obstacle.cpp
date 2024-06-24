// Copyright by Karolina Serpatowska All rights reserved


#include "Obstacle.h"
#include "RunnerPlayer.h"

// Sets default values
AObstacle::AObstacle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	obstacleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshObstacle"));
	stopBoxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionTrigger"));
	obstacleMesh->SetupAttachment(RootComponent);
	stopBoxTrigger->SetupAttachment(obstacleMesh);
	stopBoxTrigger->OnComponentBeginOverlap.AddDynamic(this, &AObstacle::BoxTriggerBeginOverlap);
	stopBoxTrigger->OnComponentEndOverlap.AddDynamic(this, &AObstacle::BoxTriggerEndOverlap);
}

// Called when the game starts or when spawned
void AObstacle::BeginPlay()
{
	Super::BeginPlay();
	
}

void AObstacle::BoxTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARunnerPlayer* player = Cast<ARunnerPlayer>(OtherActor);
	player->OnDieDelegate.ExecuteIfBound();
	UE_LOG(LogTemp, Warning, TEXT("OBSTACLE"));
}

void AObstacle::BoxTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{


}
