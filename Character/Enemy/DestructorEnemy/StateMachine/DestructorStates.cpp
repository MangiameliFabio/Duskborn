// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructorStates.h"

#include "DestructorStateMachine.h"
#include "Duskborn/Character/Enemy/DestructorEnemy/DestructorEnemy.h"
#include "Duskborn/Management/WorldManager.h"

//Idle State
//----------------------------------------------------------------------------------------------------------------------

void UDestructorIdle::Start()
{
	if (!ValidateState()) { return; }
	
	Super::Start();
	
	check(DESTRUCTOR);

	if (!DESTRUCTOR->GetWorld())
	{
		Log::Print("World not found");
		return;
	}

	//Set Timer for range Check. Will Check every second if player is in range.
	DESTRUCTOR->GetWorldTimerManager().SetTimer(DESTRUCTOR->RangeCheckTimer, this, &UDestructorIdle::PlayerInRange,
	                                            1.f, true);
}

void UDestructorIdle::PlayerInRange() const
{
	check(DESTRUCTOR);
	
	//Check if player is in Range
	if (DESTRUCTOR->CheckIfPlayerInRange())
	{
		if (!DESTRUCTOR->GetWorld())
		{
			Log::Print("World not found");
			return;
		}
		DESTRUCTOR->GetWorldTimerManager().ClearTimer(DESTRUCTOR->RangeCheckTimer);
		DESTRUCTOR_SM->StateTransition(DESTRUCTOR_SM->ActiveState);
	}


	//If destructor got hit by player immediately start charging
	if (DESTRUCTOR->GotHit)
	{
		if (!DESTRUCTOR->GetWorld())
		{
			Log::Print("World not found");
			return;
		}
		DESTRUCTOR->GetWorldTimerManager().ClearTimer(DESTRUCTOR->RangeCheckTimer);
		DESTRUCTOR_SM->StateTransition(DESTRUCTOR_SM->ActiveState);
	}
}

//Active State
//----------------------------------------------------------------------------------------------------------------------

void UDestructorActive::Start()
{
	if (!ValidateState()) { return; }
	Super::Start();

	check(DESTRUCTOR)

	DESTRUCTOR->StartCharging();
}

void UDestructorActive::End()
{
	if (!ValidateState()) { return; }
	Super::End();
}

void UDestructorActive::Update(float DeltaTime)
{
	if (!ValidateState()) { return; }
	Super::Update(DeltaTime);

	check(DESTRUCTOR)

	//Check if destructor has reloaded
	if (DESTRUCTOR->Reloaded)
	{
		DESTRUCTOR->Reloaded = false;

		//If destructor got hit recently or player is in range start charging again else go back in idle state
		if (DESTRUCTOR->GotHit || DESTRUCTOR->CheckIfPlayerInRange())
		{
			DESTRUCTOR->StartCharging();
		}
		else
		{
			DESTRUCTOR_SM->StateTransition(DESTRUCTOR_SM->IdleState);
		}
	}
	//If charging or firing a laser change size of vfx and damage object
	else if (DESTRUCTOR->Charging || DESTRUCTOR->FiringLaser)
	{
		DESTRUCTOR->ChangeLaserSize();
	}

	//If player character is not to close rotate destructor barrel towards player
	if (FVector::Dist(PLAYER->GetActorLocation(), DESTRUCTOR->GetActorLocation()) >= 1500.f)
		DESTRUCTOR->RotateToPlayer(DeltaTime);
}
