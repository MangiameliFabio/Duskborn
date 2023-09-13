// Fill out your copyright notice in the Description page of Project Settings.


#include "PartBoxSpawnPoint.h"

#include "Duskborn/Management/WorldManager.h"


// Sets default values
APartBoxSpawnPoint::APartBoxSpawnPoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void APartBoxSpawnPoint::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//Add all spawn points to world manager
	WorldManager::GetPartSpawnPoints().Add(this);
}
