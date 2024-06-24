// Copyright by Karolina Serpatowska All rights reserved


#include "RunnerEndTrigger.h"
#include "RunnerPlayer.h"

// Sets default values
ARunnerEndTrigger::ARunnerEndTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	pointsBoxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("PointsTrigger"));
	pointsBoxTrigger->SetupAttachment(RootComponent);
	pointsBoxTrigger->OnComponentBeginOverlap.AddDynamic(this, &ARunnerEndTrigger::BoxTriggerBeginOverlap);

}

// Called when the game starts or when spawned
void ARunnerEndTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARunnerEndTrigger::BoxTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARunnerPlayer* player = Cast<ARunnerPlayer>(OtherActor);
	if (player)
	{
		player->EndRun();
	}
}
