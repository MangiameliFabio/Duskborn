// Fill out your copyright notice in the Description page of Project Settings.


#include "Duskborn/Character/Player/InputManager.h"

#include "PlayerCharacter.h"
#include "Duskborn/DuskbornGameModeBase.h"
#include "Duskborn/Character/BaseClasses/StateMachine.h"
#include "Duskborn/Management/WorldManager.h"
#include "Framework/Application/SlateApplication.h"
#include "StateMachine/PlayerStateMachine.h"
#include "StateMachine/PlayerStates.h"
#include "Misc/ScopeLock.h"


void UInputManager::SendActionInput(const EInputs Input, const EInputEvent Event, const bool FromBuffer)
{
	//Check for StateMachine
	if (!Player->StateMachine)
	{
		Log::Print("StateMachine not found in Player Input Manager -> SendActionInput()");
		return;
	}

	bool handledByMovement = false;

	//Check if player can move. For example player shouldn't jump during sword attacks
	if (!Player->MovementBlocked)
	{
		//Check if Current Movement State is nullptr
		if (!Player->StateMachine->CurrentState)
		{
			Log::Print("CurrentMovementState not found in Player Input Manager -> SendActionInput()");
			return;
		}
		//Send Input to currently active player movement state
		handledByMovement = Cast<UPlayerBaseState>(Player->StateMachine->CurrentState)->HandleAction(Input, Event);
	}

	//Check if Current Combat State is nullptr
	if (!Player->StateMachine->CurrentCombatState)
	{
		Log::Print("CurrentCombatState not found in Player Input Manager -> SendActionInput()");
		return;
	}
	//Send Input to currently active player combat state
	const bool handledByCombat = Cast<UPlayerBaseState>(Player->StateMachine->CurrentCombatState)->
		HandleAction(Input, Event);

	//If the input couldn't be handled add it to the input buffer. Inputs which were already coming from the buffer shouldn't be added again
	if (!handledByMovement && !handledByCombat && !FromBuffer)
		AddToInputBuffer(Input, Event);
}

void UInputManager::SendAxisInput(EInputs Input, float AxisValue)
{
	//Check for StateMachine
	if (!Player->StateMachine)
	{
		Log::Print("StateMachine not found in Player Input Manager -> SendAxisInput()");
		return;
	}

	//Check if player can move
	if (!Player->MovementBlocked)
	{
		if (Input == EInputs::RIGHT_LEFT)
			CurrentMovementInput += Player->GetActorRightVector() * AxisValue;

		if (Input == EInputs::FORWARD_BACKWARD)
			CurrentMovementInput += Player->GetActorForwardVector() * AxisValue;

		//Check if Current Movement State is nullptr
		if (!Player->MovementBlocked && !Player->StateMachine->CurrentState)
		{
			Log::Print("CurrentMovementState not found in Player Input Manager -> SendAxisInput()");
			return;
		}
		//Send Axis Input to currently active player movement state
		Cast<UPlayerBaseState>(Player->StateMachine->CurrentState)->HandleAxisInput(
			Input, AxisValue);
	}

	//Check if Current Combat State is nullptr
	if (!Player->StateMachine->CurrentCombatState)
	{
		Log::Print("CurrentCombatState not found in Player Input Manager -> SendActionInput()");
		return;
	}
	//Send Axis Input to currently active player combat state
	Cast<UPlayerBaseState>(Player->StateMachine->CurrentCombatState)->HandleAxisInput(Input, AxisValue);
}

void UInputManager::ReceiveActionInput(EInputs Input, EInputEvent Event)
{
	if (!Player->IsDead)
	{
		//Add or remove input from the list of currently pressed inputs
		if (Event == IE_Pressed)
		{
			PressedInputs.Add(Input);
		}
		else if (Event == IE_Released)
		{
			PressedInputs.Remove(Input);
		}

		SendActionInput(Input, Event);
	}
}

void UInputManager::ReceiveAxisInput(EInputs Input, float AxisValue)
{
	if (!Player->IsDead)
		SendAxisInput(Input, AxisValue);
}

void UInputManager::ClearInputBuffer()
{
	InputBuffer.Empty();
}

FVector UInputManager::GetLastInputDirection()
{
	//Get a normalized vector of the last inputs
	if (!LastInputValue.IsNormalized())
	{
		LastInputValue.Normalize();
	}
	return LastInputValue;
}

void UInputManager::ClearCurrentInputs()
{
	PressedInputs.Empty();
}

void UInputManager::UpdateLastInputDirection()
{
	LastInputValue = CurrentMovementInput;
	CurrentMovementInput = FVector::Zero();
}

bool UInputManager::CheckIfInputIsPressed(EInputs Input)
{
	if (PressedInputs.IsEmpty()) { return false; }

	for (const auto PressedInput : PressedInputs)
	{
		if (PressedInput == Input)
		{
			return true;
		}
	}
	return false;
}

void UInputManager::AddToInputBuffer(EInputs Input, EInputEvent Event)
{
	// // If the buffer is full, remove the oldest input
	if (InputBuffer.Num() > InputBufferMaxLength - 1)
	{
		InputBuffer.RemoveAt(0);
	}

	// Add the new input to the buffer
	InputBuffer.Add(MakeTuple(Input, Event, WORLD_TIME));
}

void UInputManager::CheckInputBuffer()
{
	//This will prevent recursion
	if (!HandleInputBuffer)
	{
		HandleInputBuffer = true;

		//Loop through all items in the buffer
		for (int i = 0; i < InputBuffer.Num(); ++i)
		{
			//Send Input to currently active player states
			HandleBufferedInput(&InputBuffer[i]);
		}
		HandleInputBuffer = false;

		ClearInputBuffer();
	}
}

void UInputManager::HandleBufferedInput(FBufferTuple* Buffered)
{
	const EInputs Input = Buffered->Get<0>();
	const EInputEvent Event = Buffered->Get<1>();

	SendActionInput(Input, Event, true);
}

void UInputManager::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<APlayerCharacter>(GetOwner());

	InputBuffer.Reserve(InputBufferMaxLength);
}

void UInputManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!InputBuffer.IsEmpty())
	{
		if (WORLD_TIME - InputBuffer[0].Get<2>() >= BufferDuration)
		{
			InputBuffer.RemoveAt(0);
		}
	}
}
