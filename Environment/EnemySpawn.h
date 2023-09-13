// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemySpawn.generated.h"

enum class EEnemies : uint8;
class AEnemyCharacter;
class USphereComponent;
class ADuskbornGameModeBase;
enum class EEnemiesTest : uint8;

UCLASS()
class DUSKBORN_API AEnemySpawn : public AActor
{
	GENERATED_BODY()

	//-----------------------Variables----------------------------------------------------------------------------------  

	//public variables
public:
	UPROPERTY()
	ADuskbornGameModeBase* GameModeRef = nullptr;
	UPROPERTY(EditAnywhere)
	USphereComponent* CollisionSphere = nullptr;

	/** Class of enemy which should be spawned */
	UPROPERTY(EditAnywhere, Category="Duskborn")
	TSubclassOf<AEnemyCharacter> EnemyClass = nullptr;
	UPROPERTY(EditAnywhere, Category="Duskborn")
	EEnemies EnemyType;
	
	/** Min cooldown after which an enemy can spawn again */
	UPROPERTY(EditAnywhere, Category="Duskborn")
	float MinSpawnCoolDown = 5.f;

	/** Max cooldown after which an enemy can spawn again */
	UPROPERTY(EditAnywhere, Category="Duskborn")
	float MaxSpawnCoolDown = 20.f;

	/** Amount of enemy which should spawn */
	UPROPERTY(EditAnywhere, Category="Duskborn")
	float AmountToSpawn = 10.f;
	float SpawnedCount = 0.f;

	/** Defines the max distance to player. If the current distance to player character is to over max, enemies won't spawn */
	UPROPERTY(EditAnywhere, Category="Duskborn")
	float MaxDistanceToPlayer = 60000.f;
	/** Defines the min distance to player. If the current distance to player character is to under min, enemies won't spawn */
	UPROPERTY(EditAnywhere, Category="Duskborn")
	float MinDistanceToPlayer = 10000.f;

	/** If this bool is true enemies will be spawned all the time. AmountToSpawn is ignored. */
	UPROPERTY(EditAnywhere, Category="Duskborn")
	bool Infinite = false;

	FTimerHandle SpawnCoolDownTimer;

	//protected variables
protected:
	//private variables
private:
	
	//-----------------------Functions----------------------------------------------------------------------------------

	//public functions  
public:
	AEnemySpawn();

	void SetSpawnTimer();
	
	//protected functions
protected:
	virtual void BeginPlay() override;
	//private functions  
private:
	void SpawnEnemy();
	bool CheckDistance() const;
	bool CheckForCharactersAtSpawn() const;
	void SpawnUpdate();
	/** @return random float between Min- and MaxSpawnCooldown */
	float GetRandomSpawnCooldown() const;
};
