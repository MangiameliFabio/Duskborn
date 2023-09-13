// Fill out your copyright notice in the Description page of Project Settings.


#include "CritChanceBuff.h"

#include "Duskborn/Character/BaseClasses/BuffInstance.h"
#include "Duskborn/Management/WorldManager.h"


// Sets default values
ACritChanceBuff::ACritChanceBuff() : ABuff()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ACritChanceBuff::AddInstance(ABuffInstance* Instance)
{
	Super::AddInstance(Instance);
	
	check(PLAYER)
	PLAYER->SetCriticalStrikeChance(PLAYER->GetCriticalStrikeChance() + Instance->BuffAmount);
}

void ACritChanceBuff::InstanceRemoved(ABuffInstance* Instance)
{
	Super::InstanceRemoved(Instance);
	
	check(PLAYER)
	PLAYER->SetCriticalStrikeChance(PLAYER->GetCriticalStrikeChance() - Instance->BuffAmount);
}
