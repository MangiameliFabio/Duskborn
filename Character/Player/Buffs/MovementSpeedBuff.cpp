// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementSpeedBuff.h"

#include "AttackDamageBuff.h"
#include "Duskborn/Character/BaseClasses/BuffInstance.h"
#include "Duskborn/Management/WorldManager.h"


// Sets default values
AMovementSpeedBuff::AMovementSpeedBuff()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AMovementSpeedBuff::AddInstance(ABuffInstance* Instance)
{
	Super::AddInstance(Instance);

	check(PLAYER)
	PLAYER->SetMovementSpeed(PLAYER->GetMovementSpeed() + Instance->BuffAmount);
	PLAYER->SetSprintSpeed(PLAYER->GetSprintSpeed() + Instance->BuffAmount);
}

void AMovementSpeedBuff::InstanceRemoved(ABuffInstance* Instance)
{
	Super::InstanceRemoved(Instance);

	check(PLAYER)
	PLAYER->SetMovementSpeed(PLAYER->GetMovementSpeed() - Instance->BuffAmount);
	PLAYER->SetSprintSpeed(PLAYER->GetSprintSpeed() - Instance->BuffAmount);
}
