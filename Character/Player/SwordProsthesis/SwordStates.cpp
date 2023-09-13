// Fill out your copyright notice in the Description page of Project Settings.


#include "SwordStates.h"

#include "SkewerDamage.h"
#include "SlashDamage.h"
#include "SwordProsthesis.h"
#include "Duskborn/Character/BaseClasses/DamageObject.h"
#include "Duskborn/Character/Player/PlayerCharacter.h"
#include "Duskborn/Character/BaseClasses/StateMachine.h"
#include "Duskborn/Character/Player/StateMachine/PlayerStateMachine.h"
#include "Duskborn/Character/Player/StateMachine/PlayerStates.h"
#include "Duskborn/Management/WorldManager.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#define SWORD Cast<ASwordProsthesis>(Owner)

//Sword Slash State
//----------------------------------------------------------------------------------------------------------------------
void USwordSlash::Start()
{
	if (!ValidateState()) { return; }
	Super::Start();

	STATE_MACHINE->EnableCombatState();

	const float PlayRate = SWORD->ProsthesisAttackSpeed / PLAYER->GetAttackSpeedAsMultiplier();

	//Play anim montage for each combo state
	switch (SWORD->ComboCount)
	{
	case 0: SWORD->PlayAnimMontage(SWORD->FirstSlashMontageBody,SWORD->FirstSlashMontageLeft,
	                               SWORD->FirstSlashMontageRight, PlayRate);
		break;
	case 1: SWORD->PlayAnimMontage(SWORD->SecondSlashMontageBody,SWORD->SecondSlashMontageLeft,
	                               SWORD->SecondSlashMontageRight, PlayRate);
		break;
	case 2: SWORD->PlayAnimMontage(SWORD->ThirdSlashMontageBody,SWORD->ThirdSlashMontageLeft,
	                               SWORD->ThirdSlashMontageRight, PlayRate
		);
		break;
	default:
		break;
	}

	check(SWORD);
	SWORD->CanUsePrimary = false;
	SWORD->ComboShouldTick = false;
	SWORD->CurrentBufferDuration = 0.f;

	check(PLAYER);
	PLAYER->DisableProsthesisSwap = true;
}

void USwordSlash::End()
{
	if (StateShouldEnd)
	{
		if (!ValidateState()) { return; }
		Super::End();

		STATE_MACHINE->DisableCombatState();

		check(SWORD);
		//Count up combo or reset it
		SWORD->HandleCombo();
		SWORD->CanUsePrimary = true;

		//Destroy Damage Object
		if (SlashDamageObject)
		{
			SlashDamageObject->Destroy();
		}
		SlashDamageObject = nullptr;

		check(PLAYER);
		//Enable prosthesis swap
		PLAYER->DisableProsthesisSwap = false;

		//Handle inputs in InputBuffer
		PLAYER->InputManager->CheckInputBuffer();
	}
}

void USwordSlash::Update(float DeltaTime)
{
	if (!ValidateState()) { return; }
	Super::Update(DeltaTime);
}

bool USwordSlash::HandleAction(EInputs Input, EInputEvent Event)
{
	Super::HandleAction(Input, Event);

	return false;
}

bool USwordSlash::HandleAxisInput(EInputs Input, float AxisValue)
{
	if (!ValidateState()) { return false; }
	Super::HandleAxisInput(Input, AxisValue);

	switch (Input)
	{
	case EInputs::DASH: if (PLAYER->CanDash)
		{
			STATE_MACHINE->DeactivateCombatState();
			STATE_MACHINE->EnableMovementState();
			STATE_MACHINE->StateTransition(STATE_MACHINE->DashState);

			return true;
		}
		break;
	default: break;
	}
	return false;
}

void USwordSlash::AttackNotify()
{
	if (!ValidateState()) { return; }
	Super::AttackNotify();

	SlashDamageObject = Cast<ASlashDamage>(
		SWORD->SpawnDamageObject(SWORD->PrimaryDamageScaler, SWORD->SlashDamageClass));
	SlashDamageObject->SetComboCount(SWORD->ComboCount);
}

//Skewer State
//----------------------------------------------------------------------------------------------------------------------
void USwordSkewer::Start()
{
	if (!ValidateState()) { return; }
	Super::Start();

	STATE_MACHINE->DeactivateMovementState();
	STATE_MACHINE->EnableCombatState();

	check(SWORD);
	SWORD->SetSpecialCooldown();
	SWORD->PlayAnimMontage(SWORD->SkewerBody,SWORD->SkewerLeft,SWORD->SkewerRight);

	check(PLAYER);
	PLAYER->DetachGrapplingHook();
	PLAYER->StopPullGrapplingHook();
	PLAYER->GetCharacterMovement()->StopActiveMovement();

	//Setup Player movement 
	PLAYER->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	PLAYER->GetCharacterMovement()->MaxFlySpeed = SWORD->SkewerStrength;
	PLAYER->GetCharacterMovement()->MaxAcceleration = 1000000.f;
	PLAYER->GetCharacterMovement()->Velocity = FVector::Zero();

	//Change collision channel to enable rushing through enemies
	PLAYER->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel6, ECR_Ignore);
	PLAYER->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);

	PLAYER->bUseControllerRotationYaw = false;

	//During skewer attack prevent prostheses swap
	PLAYER->DisableProsthesisSwap = true;
}

void USwordSkewer::End()
{
	if (StateShouldEnd)
	{
		if (!ValidateState()) { return; }
		Super::End();

		//Destroy damage object
		if (SkewerDamageObject)
			SkewerDamageObject->Destroy();

		SkewerDamageObject = nullptr;

		MoveCharacter = false;

		check(SWORD);
		SWORD->SkewerShouldFinish = false;

		STATE_MACHINE->DisableCombatState();
		STATE_MACHINE->EnableMovementState();

		check(PLAYER);
		//Reset character movement
		PLAYER->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		PLAYER->GetCharacterMovement()->MaxFlySpeed = PLAYER->MaxFlySpeed;
		PLAYER->GetCharacterMovement()->MaxAcceleration = PLAYER->MaxAcceleration;

		//Reset capsule collision back to normal
		PLAYER->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel6, ECR_Block);
		PLAYER->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);

		PLAYER->bUseControllerRotationYaw = true;
		PLAYER->DisableProsthesisSwap = false;
	}
}

void USwordSkewer::Update(float DeltaTime)
{
	if (!ValidateState()) { return; }
	Super::Update(DeltaTime);

	//MoveCharacter is set to true with an animation notify. This will start skewer movement
	if (MoveCharacter)
	{
		PLAYER->AddMovementInput(AimVector);
	}
	else
	{
		//Update AimVector. Enable aiming while charging skewer
		PLAYER->GetCharacterMovement()->Velocity = FVector::Zero();
		AimVector = PLAYER->GetAimVector();
		AimVector.Normalize();
	}

	check(SWORD);
	//Finish skewer ability
	if (SWORD->SkewerShouldFinish)
	{
		if (SkewerDamageObject)
		{
			//Check if player character is currently colliding with an enemy collision
			TArray<AActor*> OverlappingActors;
			SkewerDamageObject->DamageMesh->GetOverlappingActors(OverlappingActors);
			if (!OverlappingActors.IsEmpty())
			{
				for (const auto Actor : OverlappingActors)
				{
					if (Cast<AEnemyCharacter>(Actor))
					{
						//Don't end state until no collision occured
						return;
					}
				}
			}
		}

		End();
	}
}

void USwordSkewer::AttackNotify()
{
	Super::AttackNotify();

	SkewerDamageObject = Cast<ASkewerDamage>(
		SWORD->SpawnDamageObject(SWORD->SpecialDamageScalar, SWORD->SkewerDamageClass));
	SkewerDamageObject->SetActorRotation(PLAYER->GetAimVector().Rotation());

	check(SWORD);
	SWORD->SetSkewerDuration();

	MoveCharacter = true;
}
