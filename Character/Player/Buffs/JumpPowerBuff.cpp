// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpPowerBuff.h"

#include "Duskborn/Character/BaseClasses/BuffInstance.h"
#include "Duskborn/Management/WorldManager.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AJumpPowerBuff::AJumpPowerBuff() : ABuff()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AJumpPowerBuff::AddInstance(ABuffInstance* Instance)
{
	Super::AddInstance(Instance);

	check(PLAYER)
	PLAYER->GetCharacterMovement()->JumpZVelocity = PLAYER->GetCharacterMovement()->JumpZVelocity + Instance->
		BuffAmount;
}

void AJumpPowerBuff::InstanceRemoved(ABuffInstance* Instance)
{
	Super::InstanceRemoved(Instance);

	check(PLAYER)
	PLAYER->GetCharacterMovement()->JumpZVelocity = PLAYER->GetCharacterMovement()->JumpZVelocity - Instance->
		BuffAmount;
}
