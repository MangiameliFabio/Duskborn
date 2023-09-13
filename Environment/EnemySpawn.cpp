// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawn.h"

#include "Duskborn/DuskbornGameModeBase.h"
#include "Duskborn/Character/Enemy/EnemyCharacter.h"
#include "Duskborn/Helper/Helper.h"


// Sets default values
AEnemySpawn::AEnemySpawn() : AActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>("CollisionSphere");
	SetRootComponent(CollisionSphere);
	CollisionSphere->SetSphereRadius(1000);
}

void AEnemySpawn::SetSpawnTimer()
{
	//Start cooldown for enemy spawn
	GetWorldTimerManager().SetTimer(SpawnCoolDownTimer, this, &AEnemySpawn::SpawnUpdate,
	                                GetRandomSpawnCooldown(),
	                                true);
}

// Called when the game starts or when spawned
void AEnemySpawn::BeginPlay()
{
	Super::BeginPlay();

	if (!GetWorld())
	{
		Log::Print("World not found");
		return;
	}

	//Add enemy spawn to array in game mode
	GameModeRef = Cast<ADuskbornGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameModeRef)
		GameModeRef->EnemySpawns.Add(this);
}

void AEnemySpawn::SpawnEnemy()
{
	//If the max amount of enemies to spawn is not reached ot infinite is true -> spawn enemy
	if (Infinite || SpawnedCount < AmountToSpawn)
	{
		//Spawn enemy
		AEnemyCharacter* Enemy = GetWorld()->SpawnActor<AEnemyCharacter>(
			EnemyClass, GetActorLocation(), GetActorRotation());

		if (Enemy)
			Enemy->Init(this);


		//Set cooldown again with random duration between min and max cooldwon
		GetWorldTimerManager().SetTimer(SpawnCoolDownTimer, this, &AEnemySpawn::SpawnUpdate,
		                                GetRandomSpawnCooldown(), true);
	}
	else
	{
		//Clear timer if limit is reached
		GetWorldTimerManager().ClearTimer(SpawnCoolDownTimer);
	}

	//Count up amount of enemies spawned
	SpawnedCount++;
}

void AEnemySpawn::SpawnUpdate()
{
	if (!GetWorld())
	{
		Log::Print("World not found");
		return;
	}
	check(PLAYER);

	if (
		//Check if the maximum amount of enemies in the level is reached
		GameModeRef->CheckIfEnemyCanSpawn(EnemyType)
		//Check if the distance to player character allows spawning
		&& CheckDistance()
		//Check if the spawn is not occupied by another character
		&& !CheckForCharactersAtSpawn()
		//Check if the spawn is not in FOV of the player
		&& !Helper::CheckIfLocationIsInFOV(GetWorld(), GetActorLocation()))
	{
		SpawnEnemy();
	}
}

bool AEnemySpawn::CheckDistance() const
{
	const float CurrentDistanceToPlayer = FVector::Dist(GetActorLocation(),PLAYER->GetActorLocation());
	if (CurrentDistanceToPlayer < MaxDistanceToPlayer && CurrentDistanceToPlayer > MinDistanceToPlayer)
	{
		return true;
	}

	return false;
}

bool AEnemySpawn::CheckForCharactersAtSpawn() const
{
	TArray<AActor*> OverlappingActors;
	CollisionSphere->GetOverlappingActors(OverlappingActors, ABaseCharacter::StaticClass());

	return !OverlappingActors.IsEmpty();
}

float AEnemySpawn::GetRandomSpawnCooldown() const
{
	return FMath::FRandRange(MinSpawnCoolDown, MaxSpawnCoolDown);
}
