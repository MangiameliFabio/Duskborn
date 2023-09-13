// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PartBoxManager.generated.h"

class APartLootBox;
class APartBoxSpawnPoint;

UCLASS()
class DUSKBORN_API APartBoxManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APartBoxManager();
	void Init();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APartLootBox> PartLootBoxClass = nullptr;

	/** Min distance after which a PartLootBox can spawn again */
	UPROPERTY(EditDefaultsOnly)
	float MinDistance = 0.f;
	/** Max distance after which a PartLootBox will definitely spawn */
	UPROPERTY(EditDefaultsOnly)
	float MaxDistance = 1000.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	TArray<APartBoxSpawnPoint*> PlacedSpawnPoints;
};
