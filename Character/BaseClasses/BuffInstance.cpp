// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffInstance.h"

#include "Buff.h"
#include "Duskborn/Management/WorldManager.h"

ABuffInstance::ABuffInstance() : AActor()
{
}

void ABuffInstance::Init(EStats Stat, ABuff* Parent, float Amount)
{
	StatToBuff = Stat;
	BuffParent = Parent;
	BuffAmount = Amount;
}

void ABuffInstance::SetBuffTimer(const float Duration)
{
	check(GetWorld());
	check(PLAYER);

	//After the timer of this instance is finished, it will destroy itself
	GetWorldTimerManager().SetTimer(BuffDurationTimer, this, &ABuffInstance::OnInstanceDestroyed, Duration, false);
}

void ABuffInstance::OnInstanceDestroyed()
{
	BuffParent->InstanceRemoved(this);
	CleanUp();
}

void ABuffInstance::CleanUp()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);
	Destroy();
}
