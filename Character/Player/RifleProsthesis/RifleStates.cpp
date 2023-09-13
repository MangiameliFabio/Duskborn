// Fill out your copyright notice in the Description page of Project Settings.


#include "RifleStates.h"

#include "RifleProsthesis.h"
#include "Duskborn/Character/Player/PlayerCharacter.h"
#include "Duskborn/Character/Player/StateMachine/PlayerStateMachine.h"
#include "Duskborn/Management/WorldManager.h"
#include "Kismet/GameplayStatics.h"

#define RIFLE Cast<ARifleProsthesis>(Owner)

//Rifle Fire State
//----------------------------------------------------------------------------------------------------------------------

void URifleFireState::Start()
{
	if (!ValidateState()) { return; };
	if (!RIFLE->GetWorld())
	{
		Log::Print("World not found");
		return;
	}

	Super::Start();

	STATE_MACHINE->EnableCombatState();

	RIFLE->IsShooting = true;

	//Ensure player character rotation is following the camera rotation
	PLAYER->FreeCamBlocked = true;

	RIFLE->GetWorldTimerManager().ClearTimer(RIFLE->IsShootingTimer);
}

void URifleFireState::End()
{
	if (StateShouldEnd)
	{
		if (!ValidateState()) { return; };

		Super::End();

		PLAYER->FreeCamBlocked = false;
		RIFLE->IsShooting = false;
	}
}

void URifleFireState::Update(float DeltaTime)
{
	if (!ValidateState()) { return; };
	if (!RIFLE->GetWorld())
	{
		Log::Print("World not found");
		return;
	}

	Super::Update(DeltaTime);

	//Check if input is pressed to always start firing when action is pressed
	if (PLAYER->InputManager->CheckIfInputIsPressed(EInputs::ACTION))
	{
		if (RIFLE->CanUsePrimary)
		{
			RIFLE->GetWorldTimerManager().SetTimer(RIFLE->IsShootingTimer, FTimerDelegate::CreateWeakLambda(
				                                       RIFLE, [this]()
				                                       {
					                                       PLAYER->FreeCamBlocked = false;
					                                       RIFLE->IsShooting = false;
				                                       }), 3.f, false);

			RIFLE->SetPrimaryAttackCooldown();
			FHitResult Hit;

			//Get location where camera is pointing at an set as target
			const FVector Target = PLAYER->GetPointedHit(Hit);

			RIFLE->SpawnProjectile(Target);
			RIFLE->CheckForHit(Hit);

			UGameplayStatics::PlaySoundAtLocation(RIFLE, RIFLE->ShootingSound, RIFLE->GetActorLocation());
		}
	}
}

bool URifleFireState::HandleAction(EInputs Input, EInputEvent Event)
{
	if (!ValidateState()) { return false; };

	Super::HandleAction(Input, Event);

	switch (Input)
	{
	case EInputs::ACTION:
		if (Event == IE_Released)
			STATE_MACHINE->DisableCombatState();
		if (Event == IE_Pressed)
			STATE_MACHINE->EnableCombatState();
		return true;
	default: break;
	}

	return false;
}

bool URifleFireState::HandleAxisInput(EInputs Input, float AxisValue)
{
	Super::HandleAxisInput(Input, AxisValue);

	return false;
}

//Killer Instinct State
//----------------------------------------------------------------------------------------------------------------------

void UKillerInstinctState::Start()
{
	if (!ValidateState()) { return; };

	Super::Start();

	RIFLE->PlayAnimMontage(RIFLE->KillerInstinctBody, nullptr, RIFLE->KillerInstinctRight);
	RIFLE->CanUseSpecial = false;

	PLAYER->DisableProsthesisSwap = true;
	PLAYER->BlockPrimaryAttack = true;


	STATE_MACHINE->EnableCombatState();
}

void UKillerInstinctState::End()
{
	if (StateShouldEnd)
	{
		if (!ValidateState()) { return; };
		Super::End();

		PLAYER->InputManager->CheckInputBuffer();
		PLAYER->DisableProsthesisSwap = false;
		PLAYER->BlockPrimaryAttack = false;
	}
}

void UKillerInstinctState::Update(float DeltaTime)
{
	if (!ValidateState()) { return; };

	Super::Update(DeltaTime);
}

bool UKillerInstinctState::HandleAction(EInputs Input, EInputEvent Event)
{
	Super::HandleAction(Input, Event);

	return false;
}

bool UKillerInstinctState::HandleAxisInput(EInputs Input, float AxisValue)
{
	Super::HandleAxisInput(Input, AxisValue);

	return false;
}

void UKillerInstinctState::AttackNotify()
{
	Super::AttackNotify();

	RIFLE->ActivateKillerInstinct();
	PLAYER->FinishCurrentCombatState();
}
