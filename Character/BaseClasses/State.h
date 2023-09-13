#pragma once

#include "CoreMinimal.h"
#include "Prosthesis.h"
#include "StateMachine.h"
#include "Duskborn/Enums.h"
#include "Duskborn/Helper/Log.h"
#include "State.generated.h"


class AProsthesis;

/** General state class for state machines */
UCLASS(Abstract)
class DUSKBORN_API UState : public UObject
{
	GENERATED_BODY()

	//-----------------------Variables----------------------------------------------------------------------------------  

	//public variables
public:
	UPROPERTY()
	UStateMachine* StateMachine = nullptr;

	/** Prevent state from ending behaviour if End() was already called */
	bool StateShouldEnd = false;
	
	//protected variables
protected:
	//private variables
private:
	//-----------------------Functions----------------------------------------------------------------------------------

	//public functions  
public:
	/** Initialize new State */
	virtual void Init(UStateMachine* NewStateMachine) { StateMachine = NewStateMachine; }

	/** Overloaded init if a reference to an owning prosthesis is needed */
	virtual void Init(UStateMachine* NewStateMachine, AProsthesis* OwningProsthesis)
	{
	}

	/** Called when state is entered */
	virtual void Start()
	{
		StateShouldEnd = true;
	}

	/** Called on state end */
	virtual void End()
	{
		StateShouldEnd = false;
	}

	/** Update State */
	virtual void Update(float DeltaTime)
	{
	}

	//protected functions
protected:
	/** Check if state has a valid state machine reference */
	bool ValidateState() const
	{
		if (!StateMachine)
		{
			Log::Print("No StateMachine found in State");
			return false;
		}
		return true;
	}

	//private functions  
private:
};

//-------------------Player Base State----------------------------------------------------------------------------------

/** Special player states which also have functions for Input handling */
UCLASS()
class UPlayerBaseState : public UState
{
	GENERATED_BODY()

public:
	/** Handle action inputs in State */
	virtual bool HandleAction(EInputs Input, EInputEvent Event) { return false; }
	/** Handle axis inputs in State */
	virtual bool HandleAxisInput(EInputs Input, float AxisValue) { return false; }
};

/** Special prosthesis states because they are using the player character state machine so the owning reference needs to be saved in the state*/
UCLASS()
class UProsthesisBaseState : public UPlayerBaseState
{
	GENERATED_BODY()

public:
	UPROPERTY()
	AProsthesis* Owner = nullptr;

	virtual void Init(UStateMachine* NewStateMachine, AProsthesis* OwningProsthesis) override
	{
		StateMachine = NewStateMachine;
		Owner = OwningProsthesis;
	}

	/* Used when an attack is triggered over animation notify **/
	virtual void AttackNotify()
	{
	};
};
