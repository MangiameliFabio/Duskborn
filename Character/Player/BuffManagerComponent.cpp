// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffManagerComponent.h"

#include "Duskborn/Enums.h"
#include "Duskborn/Character/BaseClasses/BuffInstance.h"
#include "Duskborn/Management/WorldManager.h"
#include "Buffs/AttackDamageBuff.h"
#include "Buffs/CritChanceBuff.h"
#include "Buffs/HealthRegBuff.h"
#include "Buffs/JumpPowerBuff.h"
#include "Buffs/MovementSpeedBuff.h"
#include "Buffs/PullSpeedBuff.h"


// Sets default values for this component's properties
UBuffManagerComponent::UBuffManagerComponent() : UActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

ABuffInstance* UBuffManagerComponent::ActivateBuff(EStats Stat, float BuffAmount)
{
	switch (Stat)
	{
	case EStats::ATTACK_DAMAGE:
		return CreateBuffInstance(AttackDamageBuff, BuffAmount, Stat);
	case EStats::CRIT_CHANCE:
		return CreateBuffInstance(CritChanceBuff, BuffAmount, Stat);
	case EStats::HP_REGENERATION:
		return CreateBuffInstance(HealthRegBuff, BuffAmount, Stat);
	case EStats::JUMP_POWER:
		return CreateBuffInstance(JumpPowerBuff, BuffAmount, Stat);
	case EStats::MOVEMENT_SPEED:
		return CreateBuffInstance(MovementSpeedBuff, BuffAmount, Stat);
	case EStats::GRAPPLING_SPEED:
		return CreateBuffInstance(PullSpeedBuff, BuffAmount, Stat);

	default:
		Log::Print("No Buff for Stat");
		break;
	}
	return nullptr;
}


void UBuffManagerComponent::CleanUp() const
{
	AttackDamageBuff->CleanUp();
	CritChanceBuff->CleanUp();
	HealthRegBuff->CleanUp();
	JumpPowerBuff->CleanUp();
	MovementSpeedBuff->CleanUp();
	PullSpeedBuff->CleanUp();
}

// Called when the game starts
void UBuffManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	//Spawn all buffs in World
	AttackDamageBuff = SpawnBuff<AAttackDamageBuff>(AttackDamageBuffClass);
	CritChanceBuff = SpawnBuff<ACritChanceBuff>(CritChanceBuffClass);
	HealthRegBuff = SpawnBuff<AHealthRegBuff>(HealthRegBuffClass);
	JumpPowerBuff = SpawnBuff<AJumpPowerBuff>(JumpPowerBuffClass);
	MovementSpeedBuff = SpawnBuff<AMovementSpeedBuff>(MovementSpeedBuffClass);
	PullSpeedBuff = SpawnBuff<APullSpeedBuff>(PullSpeedBuffClass);

	// ...
}

ABuffInstance* UBuffManagerComponent::CreateBuffInstance(ABuff* Buff, const float BuffAmount, const EStats Stat)
{
	if (Buff)
	{
		//Create a new instance for the buff
		ABuffInstance* Instance = SpawnBuffInstance<ABuffInstance>();
		Instance->Init(Stat, Buff, BuffAmount);

		//Add the instance to the buff
		Buff->AddInstance(Instance);
		return Instance;
	}

	Log::Print("Buff is undefined in CreateBuffInstance");
	return nullptr;
}
