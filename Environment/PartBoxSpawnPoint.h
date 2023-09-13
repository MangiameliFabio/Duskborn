﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PartBoxSpawnPoint.generated.h"

UCLASS()
class DUSKBORN_API APartBoxSpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APartBoxSpawnPoint();

protected:
	virtual void PostInitializeComponents() override;
};