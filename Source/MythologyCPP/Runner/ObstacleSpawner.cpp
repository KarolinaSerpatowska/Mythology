// Copyright by Karolina Serpatowska All rights reserved


#include "ObstacleSpawner.h"
#include "RunnerPlayer.h"

// Sets default values
AObstacleSpawner::AObstacleSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	rootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(rootSceneComponent);
	posObstacle1 = CreateDefaultSubobject<USceneComponent>(TEXT("PositionObstacle1"));
	posObstacle2 = CreateDefaultSubobject<USceneComponent>(TEXT("PositionObstacle2"));
	posObstacle3 = CreateDefaultSubobject<USceneComponent>(TEXT("PositionObstacle3"));
	posObstacle4 = CreateDefaultSubobject<USceneComponent>(TEXT("PositionObstacle4"));
	posObstacle5 = CreateDefaultSubobject<USceneComponent>(TEXT("PositionObstacle5"));
	posObstacle1->SetupAttachment(rootSceneComponent);
	posObstacle2->SetupAttachment(rootSceneComponent);
	posObstacle3->SetupAttachment(rootSceneComponent);
	posObstacle4->SetupAttachment(rootSceneComponent);
	posObstacle5->SetupAttachment(rootSceneComponent);
	
	pointsBoxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("PointsTrigger"));
	pointsBoxTrigger->SetupAttachment(rootSceneComponent);
	pointsBoxTrigger->OnComponentBeginOverlap.AddDynamic(this, &AObstacleSpawner::BoxTriggerBeginOverlap);
}

// Called when the game starts or when spawned
void AObstacleSpawner::BeginPlay()
{
	Super::BeginPlay();
}

void AObstacleSpawner::BoxTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARunnerPlayer* player = Cast<ARunnerPlayer>(OtherActor);
	if (player)
	{
		player->GivePoints();
	}
}

void AObstacleSpawner::SpawnObstacles()
{
	positionsForSpawn.Add(posObstacle1->GetComponentLocation());
	positionsForSpawn.Add(posObstacle2->GetComponentLocation());
	positionsForSpawn.Add(posObstacle3->GetComponentLocation());
	positionsForSpawn.Add(posObstacle4->GetComponentLocation());
	positionsForSpawn.Add(posObstacle5->GetComponentLocation());

	int howManyObstacles = FMath::RandRange(1, maxNumberOfObstacles);

	TArray<int> generatedIndex;

	for (int i = 0; i < howManyObstacles; i++)
	{
		FVector location = positionsForSpawn[i];
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		int obstacleTypeIndex = FMath::RandRange(0, obstaclesTypesArray.Num() - 1);
		AObstacle* obstacle = GetWorld()->SpawnActor<AObstacle>(obstaclesTypesArray[obstacleTypeIndex], location, FRotator::ZeroRotator, SpawnParams);
	}
}