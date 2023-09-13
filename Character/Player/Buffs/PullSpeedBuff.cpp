// Fill out your copyright notice in the Description page of Project Settings.

#include "PullSpeedBuff.h"

#include "Duskborn/Character/BaseClasses/BuffInstance.h"
#include "Duskborn/Management/WorldManager.h"


// Sets default values
APullSpeedBuff::APullSpeedBuff()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void APullSpeedBuff::AddInstance(ABuffInstance* Instance)
{
	Super::AddInstance(Instance);

	check(PLAYER)
	PLAYER->SetPullSpeed(PLAYER->GetPullSpeed() + Instance->BuffAmount);
}

void APullSpeedBuff::InstanceRemoved(ABuffInstance* Instance)
{
	Super::InstanceRemoved(Instance);

	check(PLAYER)
	PLAYER->SetPullSpeed(PLAYER->GetPullSpeed() - Instance->BuffAmount);
}
