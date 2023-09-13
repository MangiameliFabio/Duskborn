// Fill out your copyright notice in the Description page of Project Settings.


#include "PartBoxManager.h"

#include "PartBoxSpawnPoint.h"
#include "PartLootBox.h"
#include "Duskborn/Management/WorldManager.h"


// Sets default values
APartBoxManager::APartBoxManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APartBoxManager::BeginPlay()
{
	Super::BeginPlay();
}

void APartBoxManager::Init()
{
	//Calculate difference between max and min distance
	const float DistanceDiff = MaxDistance - MinDistance;

	//Loop through all SpawnPoints in World
	for (int i = 0; i < WorldManager::GetPartSpawnPoints().Num(); ++i)
	{
		if (!WorldManager::GetPartSpawnPoints().IsEmpty())
		{
			//Choose a random spawn point
			APartBoxSpawnPoint* SpawnPoint = WorldManager::GetPartSpawnPoints()[FMath::RandRange(
				0, WorldManager::GetPartSpawnPoints().Num() - 1)];

			if (!SpawnPoint)
			{
				Log::Print("Couldn't find spawn point in Loot Box Manager");
				return;
			}

			//Initialize closest distance
			float ClosestDistance = MaxDistance * 2.f;

			//Find closest distance to another spawn point
			for (int j = 0; j < PlacedSpawnPoints.Num(); ++j)
			{
				const float Distance = FVector::Dist(PlacedSpawnPoints[j]->GetActorLocation(),
				                                     SpawnPoint->GetActorLocation());
				if (ClosestDistance > Distance)
					ClosestDistance = Distance;
			}

			//Subtract MinDistance. If result is below 0 use 0 instead
			ClosestDistance = FMath::Max(0, ClosestDistance - MinDistance);

			//Calculate spawn Chance
			if (const float SpawnChance = ClosestDistance / DistanceDiff; SpawnChance > FMath::FRandRange(0.f, 1.f))
			{
				if (!PartLootBoxClass)
				{
					Log::Print("No Part Loot Box class in Manager");
					return;
				}
				//Spawn loot box at spawn point and
				APartLootBox* PartLootBox = GetWorld()->SpawnActor<APartLootBox>(PartLootBoxClass);
				PartLootBox->SetActorLocation(SpawnPoint->GetActorLocation());
				PartLootBox->SetActorRotation(SpawnPoint->GetActorRotation());

				//Save spawn point
				PlacedSpawnPoints.Add(SpawnPoint);
			}
			else
			{
				//Remove spawn point from list
				WorldManager::GetPartSpawnPoints().Remove(SpawnPoint);
			}
		}
	}
}

// Called every frame
void APartBoxManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
