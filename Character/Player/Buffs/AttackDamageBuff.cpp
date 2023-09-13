// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackDamageBuff.h"

#include "Duskborn/Character/BaseClasses/BuffInstance.h"
#include "Duskborn/Management/WorldManager.h"


// Sets default values
AAttackDamageBuff::AAttackDamageBuff() : ABuff()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AAttackDamageBuff::AddInstance(ABuffInstance* Instance)
{
	Super::AddInstance(Instance);

	check(PLAYER)
	PLAYER->SetAttackDamage(PLAYER->GetAttackDamage() + Instance->BuffAmount);
}

void AAttackDamageBuff::InstanceRemoved(ABuffInstance* Instance)
{
	Super::InstanceRemoved(Instance);
	PLAYER->SetAttackDamage(PLAYER->GetAttackDamage() - Instance->BuffAmount);
}

