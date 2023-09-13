// Fill out your copyright notice in the Description page of Project Settings.


#include "Duskborn/Character/Player/StateMachine/PlayerStates.h"

#include "PlayerStateMachine.h"
#include "Duskborn/DuskbornGameModeBase.h"
#include "Duskborn/Character/Player/PlayerCharacter.h"
#include "Duskborn/Enums.h"
#include "Duskborn/Character/Player/GrapplingHook.h"
#include "Duskborn/Management/WorldManager.h"
#include "Duskborn/UpgradeSystem/TriggerManager.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

//GroundMovement State
//----------------------------------------------------------------------------------------------------------------------

void UPlayerGroundMovement::Start()
{
}

void UPlayerGroundMovement::End()
{
	if (!PLAYER)
	{
		Log::Print("No Player found in Ground Movement");
		return;
	}
	PLAYER->StopSprinting();
}

void UPlayerGroundMovement::Update(float DeltaTime)
{
}

bool UPlayerGroundMovement::HandleAction(const EInputs Input, const EInputEvent Event)
{
	Super::HandleAction(Input, Event);
	if (!ValidateState()) { return false; }
	if (!PLAYER)
	{
		Log::Print("No Player found in Ground Movement");
		return false;
	}
	switch (Input)
	{
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::ACTION:
		if (Event == IE_Pressed && PLAYER->ActiveProsthesis->CanUsePrimary && !PLAYER->
			BlockPrimaryAttack)
		{
			//Use Active prostheses primary attack state in state machine
			STATE_MACHINE->CombatStateTransition(
				PLAYER->ActiveProsthesis->GetPrimaryAttackState());
			return true;
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::SPECIAL:
		if (Event == IE_Pressed)
		{
			if (PLAYER->ActiveProsthesis->CanUseSpecial)
			{
				//Use Active prostheses special attack state in state machine
				STATE_MACHINE->CombatStateTransition(
					PLAYER->ActiveProsthesis->GetSpecialState());
				return true;
			}
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::JUMP:
		if (Event == IE_Pressed)
		{
			//If player character is falling and has JetPack fuel transition to JetPackState
			if (PLAYER->GetCurrentJetPackFuel() > 0.f && PLAYER->GetCharacterMovement()->IsFalling())
			{
				STATE_MACHINE->StateTransition(STATE_MACHINE->JetPackState);
				return true;
			}

			if (PLAYER->CanJump())
			{
				PLAYER->Jump();
				return true;
			}
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::SECONDARY_ACTION:
		if (Event == IE_Pressed && PLAYER->FindGrapplingPoint())
		{
			//If grappling point has been found transition into GrapplingState
			STATE_MACHINE->StateTransition(STATE_MACHINE->GrapplingState);

			return true;
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::SPRINT:
		if (Event == IE_Pressed && PLAYER->GetHorizontalVelocity().Length() > 0.f)
		{
			//Transition to SprintState if player character is moving in XY direction
			STATE_MACHINE->DisableCombatState();
			STATE_MACHINE->StateTransition(STATE_MACHINE->SprintState);

			return true;
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::DASH:
		if (Event == IE_Pressed)
		{
			if (PLAYER->CanDash)
			{
				STATE_MACHINE->StateTransition(STATE_MACHINE->DashState);

				return true;
			}
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::ABILITY_SWAP:
		if (Event == IE_Pressed)
		{
			if (PLAYER->ProsthesisSwap())
				return true;
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	default: break;
	}

	return false;
}

bool UPlayerGroundMovement::HandleAxisInput(const EInputs Input, const float AxisValue)
{
	Super::HandleAxisInput(Input, AxisValue);
	if (!ValidateState()) { return false; }

	if (!PLAYER)
	{
		Log::Print("No Player found in Ground Movement");
		return false;
	}

	switch (Input)
	{
	case EInputs::RIGHT_LEFT: PLAYER->MoveRight(AxisValue);
		return true;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::FORWARD_BACKWARD: PLAYER->MoveForward(AxisValue);
		return true;
	//------------------------------------------------------------------------------------------------------------------
	default: break;
	}

	return false;
}

//Grappling State
//----------------------------------------------------------------------------------------------------------------------

void UPlayerGrappling::Start()
{
	Super::Start();
	if (!ValidateState()) { return; }
	if (!PLAYER)
	{
		Log::Print("No Player found in Grappling State");
		return;
	}

	//Initialize use of the grappling hook
	PLAYER->IsGrappling = true;
	PLAYER->AttachRopeToGrapplingPoint();
	PLAYER->CalcRopeLength();
	PLAYER->EnableRope();
	PLAYER->ShortenRope = true;
	PLAYER->ActivateGrapplingParticle();
}

void UPlayerGrappling::End()
{
	Super::End();
	if (!ValidateState()) { return; }

	//Disable use of grappling hook
	PLAYER->DetachPhysicsHook();
	PLAYER->DeactivateGrapplingParticle();

	//Prevent trigger from being updated in Trigger Manager
	TRIGGER_MANAGER->DeactivateTrigger(ETriggerTypes::WHILE_SWINGING);
	// TRIGGER_MANAGER->DeactivateTrigger(ETriggerTypes::AFTER_SWING_TIME);
}

void UPlayerGrappling::Update(const float DeltaTime)
{
	Super::Update(DeltaTime);
	if (!ValidateState()) { return; }
	if (!PLAYER->GetWorld())
	{
		Log::Print("World not found Grappling State");
		return;
	}

	PLAYER->AttachRopeToGrapplingPoint();

	// Check conditions for rope manipulation and grappling hook usage
	// if player character movement component is currently not in walking mode
	// if player character velocity is above max speed. This is used to still be able to walk when using the grappling hook on the ground. -1 is to prevent false positive if Velocity length is something like 600.001
	// if the player is pulling in grappling point direction
	// One condition has to be true fro grappling behaviour
	if (PLAYER->GetCharacterMovement()->MovementMode != MOVE_Walking ||
		(PLAYER->GetCharacterMovement()->Velocity.Length() - 1.f) > PLAYER->GetCharacterMovement()->MaxWalkSpeed ||
		PLAYER->IsPulling)
	{
		if (PLAYER->ShortenRope)
		{
			PLAYER->ShortenRopeLength();
		}
		//Calculate the difference between RopeLength and the actual distance between player character and GrapplingPoint
		PLAYER->CalcRopeOffset();

		if (!PLAYER->UsingPhysicsHook && !PLAYER->IsPulling)
		{
			//If physics hook is not already or player character is pulling check if the offset is to large
			if (PLAYER->RopeOffset > RopeTolerance) //Tolerance to make sure physics hook is not directly used
			{
				//If the offset is to large it means the player character needs to stop falling and should start swinging with the grappling hook.
				//That's why the player needs to be "attached" to BP_GrapplingHook
				PLAYER->AttachPhysicsHook();
			}
		}
		if (PLAYER->IsPulling)
		{
			//If the player is pulling the player character can be detached from BP_GrapplingHook
			if (PLAYER->UsingPhysicsHook)
				PLAYER->DetachPhysicsHook();

			//Pull towards grappling point
			PLAYER->PullGrapplingHook();
		}
		//If the downforce of the player character is larger than the centripetal force of the character, detach from BP_GrapplingHook
		//This is needed because the constraint component of unreal is more like a rod than a rope and CheckDownForce will generate a more realistic result
		if (PLAYER->RopeOffset < RopeTolerance && PLAYER->UsingPhysicsHook && PLAYER->GrapplingHook->CheckDownForce())
		{
			Log::Print(PLAYER->RopeOffset);
			PLAYER->DetachPhysicsHook();
		}
		if (PLAYER->UsingPhysicsHook)
		{
			//BP_Grappling hook will generate the physical movement of an object on a circular path. To make the player character
			//follow, there velocity is set to the velocity of BP_GrapplingHook AttachmentSphere.
			PLAYER->GetCharacterMovement()->Velocity =
				PLAYER->GrapplingHook->PlayerAttachmentSphere->GetPhysicsLinearVelocity();

			//This Interpolation is used to prevent falling down when the character player is just hanging with the grappling hook.
			const FVector InterpolatedLocation =
				FMath::VInterpTo(PLAYER->GetActorLocation(),
				                 PLAYER->GrapplingHook->PlayerAttachmentSphere->GetComponentLocation(),
				                 DeltaTime, 10);

			FHitResult Sweep;
			//Set player character location to new interpolated value
			PLAYER->SetActorLocation(InterpolatedLocation, true, &Sweep, ETeleportType::TeleportPhysics);

			//If the sweep result is detecting a collision the player character should directly stop following BP_GrapplingHook.
			//This will prevent glitching into walls. Especially when the player character is close to the celling
			if (Sweep.bBlockingHit)
			{
				PLAYER->DetachPhysicsHook();
			}
		}
	}
	else if (PLAYER->UsingPhysicsHook)
	{
		PLAYER->DetachPhysicsHook();
	}

	if (!PLAYER->IsPulling)
	{
		//If the player character is not pulling but the sprint input is still pressed, start pulling
		if (PLAYER->InputManager->CheckIfInputIsPressed(EInputs::SPRINT))
			PLAYER->InitPullGrapplingHook();
	}

	//Check if player character is currently swinging to activate trigger in trigger manager
	if (PLAYER->CheckIfSwinging())
	{
		TRIGGER_MANAGER->ActivateTrigger(ETriggerTypes::WHILE_SWINGING);
		// TRIGGER_MANAGER->FillTriggerThreshold(ETriggerTypes::AFTER_SWING_TIME, DeltaTime);
	}
	else
	{
		TRIGGER_MANAGER->DeactivateTrigger(ETriggerTypes::WHILE_SWINGING);
		// TRIGGER_MANAGER->DeactivateTrigger(ETriggerTypes::AFTER_SWING_TIME);
	}
}

bool UPlayerGrappling::HandleAction(const EInputs Input, EInputEvent Event)
{
	Super::HandleAction(Input, Event);
	if (!ValidateState()) { return false; }

	switch (Input)
	{
	case EInputs::SPECIAL:
		if (Event == IE_Pressed)
		{
			if (PLAYER->ActiveProsthesis->CanUseSpecial)
			{
				//Use active prostheses special attack state in state machine
				STATE_MACHINE->CombatStateTransition(
					PLAYER->ActiveProsthesis->GetSpecialState());
			}
			return true;
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::ACTION:
		if (Event == IE_Pressed && PLAYER->ActiveProsthesis->CanUsePrimary && !PLAYER->BlockPrimaryAttack)
		{
			//Use active prostheses primary attack state in state machine
			STATE_MACHINE->CombatStateTransition(
				PLAYER->ActiveProsthesis->GetPrimaryAttackState());

			return true;
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::JUMP:
		if (Event == IE_Pressed)
		{
			//If player character is falling and has JetPack fuel transition to JetPackState
			if (PLAYER->GetCurrentJetPackFuel() > 0.f && !PLAYER->GetCharacterMovement()->IsMovingOnGround())
			{
				PLAYER->StopPullGrapplingHook();
				PLAYER->DetachGrapplingHook();
				STATE_MACHINE->StateTransition(STATE_MACHINE->JetPackState);
				return true;
			}

			if (PLAYER->GetCharacterMovement()->IsWalking())
			{
				if (PLAYER->CanJump())
				{
					PLAYER->Jump();
					return true;
				}
			}
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::SECONDARY_ACTION:
		if (Event == IE_Released)
		{
			PLAYER->StopPullGrapplingHook();
			PLAYER->DetachGrapplingHook();
			STATE_MACHINE->StateTransition(STATE_MACHINE->GroundMovementState);
			return true;
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::SPRINT:
		if (Event == IE_Pressed)
		{
			PLAYER->InitPullGrapplingHook();
			return true;
		}

		if (Event == IE_Released)
		{
			PLAYER->StopPullGrapplingHook();
			return true;
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::DASH:
		if (Event == IE_Pressed)
		{
			if (PLAYER->CanDash)
			{
				STATE_MACHINE->StateTransition(STATE_MACHINE->GrapplingDashState);
				return true;
			}
		}
		break;
	case EInputs::ABILITY_SWAP:
		if (Event == IE_Pressed)
		{
			if (PLAYER->ProsthesisSwap())
				return true;
		}
		break;

	//------------------------------------------------------------------------------------------------------------------
	default: break;
	}
	return false;
}

bool UPlayerGrappling::HandleAxisInput(const EInputs Input, const float AxisValue)
{
	Super::HandleAxisInput(Input, AxisValue);
	if (!ValidateState()) { return false; }

	switch (Input)
	{
	case EInputs::RIGHT_LEFT:
		//To still have control over the direction while following BP_GrapplingHook. There are Functions to
		//influence the AttachmentSphere in BP_GrapplingHook

		if (PLAYER->GetCharacterMovement()->IsMovingOnGround())
		{
			PLAYER->MoveRight(AxisValue);
			return true;
		}
		if (PLAYER->IsPulling)
		{
			PLAYER->ApplyForceRight(AxisValue);
			return true;
		}
		if (PLAYER->UsingPhysicsHook)
		{
			PLAYER->SwingRight(AxisValue);
			return true;
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::FORWARD_BACKWARD:
		if (PLAYER->GetCharacterMovement()->IsMovingOnGround())
		{
			PLAYER->MoveForward(AxisValue);
			return true;
		}
		if (PLAYER->IsPulling)
		{
			PLAYER->ApplyForceUp(AxisValue);
			return true;
		}
		if (PLAYER->UsingPhysicsHook)
		{
			PLAYER->SwingForward(AxisValue);
			return true;
		}
		break;

	//------------------------------------------------------------------------------------------------------------------
	default: break;
	}
	return false;
}

//Dash State
//----------------------------------------------------------------------------------------------------------------------

void UPlayerDash::Start()
{
	Super::Start();
	if (!ValidateState()) { return; }

	UGameplayStatics::PlaySoundAtLocation(PLAYER, PLAYER->DashSound, PLAYER->GetActorLocation());

	PLAYER->InitDash();

	STATE_MACHINE->DisableCombatState();
}

void UPlayerDash::End()
{
	Super::End();
	if (!ValidateState()) { return; }

	//If action is still pressed and primary is not blocked enable combat state
	if (PLAYER->InputManager->CheckIfInputIsPressed(EInputs::ACTION))
	{
		STATE_MACHINE->EnableCombatState();
	}

	PLAYER->ResetAfterDash();
}

void UPlayerDash::Update(float DeltaTime)
{
	Super::Update(DeltaTime);
	if (!ValidateState()) { return; }

	if (PLAYER->CurrentDashDuration >= 0.f)
	{
		//Use dash direction as input
		PLAYER->GetCharacterMovement()->AddInputVector(PLAYER->DashDirection);
	}
	else
	{
		//Check in which state should be transitioned
		if (PLAYER->InputManager->CheckIfInputIsPressed(EInputs::SPRINT))
		{
			STATE_MACHINE->StateTransition(STATE_MACHINE->SprintState);
		}
		else
		{
			STATE_MACHINE->StateTransition(STATE_MACHINE->GroundMovementState);
		}

		if (!PLAYER->GetWorld())
		{
			Log::Print("World not found");
			return;
		}
		if (!TRIGGER_MANAGER)
		{
			Log::Print("No Trigger Manager found in Dash State");
			return;
		}
		//Notify Items subscribed to OnDash trigger
		TRIGGER_MANAGER->Notify(PLAYER, ETriggerTypes::ON_DASH);
	}
	PLAYER->CurrentDashDuration -= DeltaTime;
}

bool UPlayerDash::HandleAction(EInputs Input, EInputEvent Event)
{
	Super::HandleAction(Input, Event);
	if (!ValidateState()) { return false; }

	switch (Input)
	{
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::SECONDARY_ACTION:
		if (Event == IE_Released)
		{
			PLAYER->DetachGrapplingHook();
			return true;
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::ABILITY_SWAP:
		if (Event == IE_Pressed)
		{
			if (PLAYER->ProsthesisSwap())
				return true;
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	default:
		break;
	}
	return false;
}

bool UPlayerDash::HandleAxisInput(EInputs Input, float AxisValue)
{
	Super::HandleAxisInput(Input, AxisValue);

	return false;
}

//GrapplingDash State
//----------------------------------------------------------------------------------------------------------------------

void UPlayerGrapplingDash::Start()
{
	Super::Start();
	if (!ValidateState()) { return; }

	PLAYER->InitDash();

	//The grappling dash state is similar to the normal dash but the velocity after is set to the velocity before dash.
	VelocityBeforeDash = PLAYER->GetCharacterMovement()->Velocity;
	STATE_MACHINE->DisableCombatState();
}

void UPlayerGrapplingDash::End()
{
	Super::End();
	if (!ValidateState()) { return; }

	//Reset Character Movement
	PLAYER->ResetAfterDash();
	PLAYER->GetCharacterMovement()->Velocity = PLAYER->DashDirection * PLAYER->DashStrength;
}

void UPlayerGrapplingDash::Update(float DeltaTime)
{
	Super::Update(DeltaTime);
	if (!ValidateState()) { return; }


	//Another difference to the normal dash is that in this state the end position of the rope is getting updated
	PLAYER->AttachRopeToGrapplingPoint();
	PLAYER->CalcRopeLength();

	if (PLAYER->CurrentDashDuration >= 0.f)
	{
		//Use dash direction as input
		PLAYER->GetCharacterMovement()->AddInputVector(PLAYER->DashDirection);
	}
	else
	{
		//Check in which state should be transitioned
		if (PLAYER->IsGrappling)
		{
			STATE_MACHINE->StateTransition(STATE_MACHINE->GrapplingState);
		}
		else
		{
			STATE_MACHINE->StateTransition(STATE_MACHINE->GroundMovementState);
		}
		if (!TRIGGER_MANAGER)
		{
			Log::Print("No Trigger Manager found in Dash State");
			return;
		}
		//Notify Items subscribed to OnDash trigger
		TRIGGER_MANAGER->Notify(PLAYER, ETriggerTypes::ON_DASH);
	}
	PLAYER->CurrentDashDuration -= DeltaTime;
}

bool UPlayerGrapplingDash::HandleAction(EInputs Input, EInputEvent Event)
{
	Super::HandleAction(Input, Event);
	if (!ValidateState()) { return false; }

	switch (Input)
	{
	case EInputs::SPRINT:
		if (Event == IE_Released)
		{
			//Player character shouldn't be pulled if the button was released during the dash
			PLAYER->StopPullGrapplingHook();
			return true;
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::SECONDARY_ACTION:
		if (Event == IE_Released)
		{
			PLAYER->DetachGrapplingHook();
			return true;
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::ABILITY_SWAP:
		if (Event == IE_Pressed)
		{
			if (PLAYER->ProsthesisSwap())
				return true;
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	default:
		break;
	}
	return false;
}

bool UPlayerGrapplingDash::HandleAxisInput(EInputs Input, float AxisValue)
{
	Super::HandleAxisInput(Input, AxisValue);

	return false;
}

//JetPack State
//----------------------------------------------------------------------------------------------------------------------

void UPlayerJetPack::Start()
{
	Super::Start();
	if (!ValidateState()) { return; }

	PLAYER->InitJetPack();
}

void UPlayerJetPack::End()
{
	Super::End();
	if (!ValidateState()) { return; }
	PLAYER->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	PLAYER->GetCharacterMovement()->MaxFlySpeed = PLAYER->MaxFlySpeed;
	PLAYER->DeactivateThrusterParticle();
	PLAYER->DeactivateThrusterSound();

	PLAYER->StartRefuelCooldown();
}

void UPlayerJetPack::Update(float DeltaTime)
{
	Super::Update(DeltaTime);
	if (!ValidateState()) { return; }

	//If player character has no fuel. JetPack can't be used.
	if (PLAYER->GetCurrentJetPackFuel() > 0)
	{
		//Deplete fuel
		PLAYER->SetCurrentJetPackFuel(PLAYER->GetCurrentJetPackFuel() - DeltaTime * PLAYER->FuelDepletionRate);

		//Add movement in up direction
		PLAYER->GetCharacterMovement()->AddInputVector(FVector(0, 0, 1.f));
	}
	else
	{
		STATE_MACHINE->StateTransition(STATE_MACHINE->GroundMovementState);
	}
}

bool UPlayerJetPack::HandleAction(EInputs Input, EInputEvent Event)
{
	Super::HandleAction(Input, Event);
	if (!ValidateState()) { return false; }

	switch (Input)
	{
	case EInputs::DASH:
		if (Event == IE_Pressed)
		{
			if (PLAYER->CanDash)
			{
				STATE_MACHINE->StateTransition(STATE_MACHINE->DashState);
				return true;
			}
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::SPECIAL:
		if (Event == IE_Pressed)
		{
			if (PLAYER->ActiveProsthesis->CanUseSpecial)
			{
				STATE_MACHINE->CombatStateTransition(
					PLAYER->ActiveProsthesis->GetSpecialState());
			}
			return true;
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::ACTION:
		if (Event == IE_Pressed && PLAYER->ActiveProsthesis->CanUsePrimary && !PLAYER->BlockPrimaryAttack)
		{
			STATE_MACHINE->CombatStateTransition(
				PLAYER->ActiveProsthesis->GetPrimaryAttackState());
			return true;
		}
		break;
	case EInputs::JUMP:
		if (Event == IE_Released)
		{
			STATE_MACHINE->StateTransition(STATE_MACHINE->GroundMovementState);
			return true;
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::ABILITY_SWAP:
		if (Event == IE_Pressed)
		{
			if (PLAYER->ProsthesisSwap())
				return true;
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	default:
		break;
	}

	return false;
}

bool UPlayerJetPack::HandleAxisInput(EInputs Input, float AxisValue)
{
	Super::HandleAxisInput(Input, AxisValue);
	if (!ValidateState()) { return false; }

	switch (Input)
	{
	case EInputs::RIGHT_LEFT: PLAYER->MoveRight(AxisValue);
		return true;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::FORWARD_BACKWARD: PLAYER->MoveForward(AxisValue);
		return true;
	//------------------------------------------------------------------------------------------------------------------
	default:
		break;
	//------------------------------------------------------------------------------------------------------------------
	}
	return false;
}

//Sprint State
//----------------------------------------------------------------------------------------------------------------------

void UPlayerSprintState::Start()
{
	if (!ValidateState()) { return; };

	Super::Start();

	PLAYER->StartSprinting();
}

void UPlayerSprintState::End()
{
	if (!ValidateState()) { return; };

	Super::End();
	PLAYER->StopSprinting();

	//Check if after sprinting action is still pressed to continue attacking again
	if (PLAYER->InputManager->CheckIfInputIsPressed(EInputs::ACTION))
	{
		STATE_MACHINE->EnableCombatState();
	}
}

void UPlayerSprintState::Update(float DeltaTime)
{
	if (!ValidateState()) { return; };

	Super::Update(DeltaTime);
}

bool UPlayerSprintState::HandleAction(EInputs Input, EInputEvent Event)
{
	if (!ValidateState()) { return false; };

	Super::HandleAction(Input, Event);

	switch (Input)
	{
	case EInputs::JUMP: if (Event == IE_Pressed)
		{
			//Enable jump and jetpack during sprinting
			if (PLAYER->GetCurrentJetPackFuel() > 0.f && PLAYER->GetCharacterMovement()->IsFalling())
			{
				STATE_MACHINE->StateTransition(STATE_MACHINE->JetPackState);
				return true;
			}
			if (PLAYER->CanJump())
			{
				PLAYER->Jump();
				return true;
			}
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::DASH:
		if (Event == IE_Pressed && PLAYER->CanDash)
		{
			STATE_MACHINE->StateTransition(STATE_MACHINE->DashState);
			return true;
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::SPECIAL:
		if (Event == IE_Pressed && PLAYER->ActiveProsthesis->CanUseSpecial)
		{
			STATE_MACHINE->CombatStateTransition(
					PLAYER->ActiveProsthesis->GetSpecialState());
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::SPRINT:
		if (Event == IE_Released)
		{
			STATE_MACHINE->StateTransition(
				STATE_MACHINE->GroundMovementState);
			return true;
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::ACTION:
		if (Event == IE_Pressed && !PLAYER->BlockPrimaryAttack)
		{
			STATE_MACHINE->StateTransition(
				STATE_MACHINE->GroundMovementState);
			STATE_MACHINE->CombatStateTransition(
				PLAYER->ActiveProsthesis->GetPrimaryAttackState());
			return true;
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::SECONDARY_ACTION: if (Event == IE_Pressed && PLAYER->FindGrapplingPoint())
		{
			PLAYER->InitPullGrapplingHook();
			STATE_MACHINE->StateTransition(STATE_MACHINE->GrapplingState);
			return true;
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::ABILITY_SWAP:
		if (Event == IE_Pressed)
		{
			if (PLAYER->ProsthesisSwap())
				return true;
		}
		break;
	//------------------------------------------------------------------------------------------------------------------
	default: break;
	}

	return false;
}

bool UPlayerSprintState::HandleAxisInput(EInputs Input, float AxisValue)
{
	if (!ValidateState()) { return false; };

	Super::HandleAxisInput(Input, AxisValue);

	switch (Input)
	{
	case EInputs::FORWARD_BACKWARD:
		PLAYER->MoveForwardSprint(AxisValue);
		return true;
	//------------------------------------------------------------------------------------------------------------------
	case EInputs::RIGHT_LEFT:
		PLAYER->TurnCharacter(AxisValue);
		return true;
	//------------------------------------------------------------------------------------------------------------------
	default: break;
	}

	return false;
}
