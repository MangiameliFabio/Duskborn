// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthRegBuff.h"

#include "Duskborn/Character/BaseClasses/BuffInstance.h"
#include "Duskborn/Management/WorldManager.h"

// Sets default values
AHealthRegBuff::AHealthRegBuff()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AHealthRegBuff::AddInstance(ABuffInstance* Instance)
{
	Super::AddInstance(Instance);

	check(PLAYER)
	PLAYER->SetHealthRegeneration(PLAYER->GetHealthRegeneration() + Instance->BuffAmount);
}

void AHealthRegBuff::InstanceRemoved(ABuffInstance* Instance)
{
	Super::InstanceRemoved(Instance);
	PLAYER->SetHealthRegeneration(PLAYER->GetHealthRegeneration() - Instance->BuffAmount);
}
