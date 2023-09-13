// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerStateMachine.h"

#include "PlayerStates.h"
#include "Duskborn/Character/BaseClasses/Prosthesis.h"
#include "Duskborn/Character/Player/PlayerCharacter.h"
#include "Duskborn/Management/WorldManager.h"

// Sets default values for this component's properties
UPlayerStateMachine::UPlayerStateMachine() : UStateMachine()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlayerStateMachine::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<APlayerCharacter>(GetOwner());

	GroundMovementState = NewObject<UPlayerGroundMovement>();
	GroundMovementState->Init(this);
	GrapplingState = NewObject<UPlayerGrappling>();
	GrapplingState->Init(this);
	DashState = NewObject<UPlayerDash>();
	DashState->Init(this);
	GrapplingDashState = NewObject<UPlayerGrapplingDash>();
	GrapplingDashState->Init(this);
	JetPackState = NewObject<UPlayerJetPack>();
	JetPackState->Init(this);
	SprintState = NewObject<UPlayerSprintState>();
	SprintState->Init(this);
}

void UPlayerStateMachine::Init()
{
	Super::Init();

	check(GroundMovementState)

	// In case of the player the CurrentState is also referred as movement state
	CurrentState = GroundMovementState;
	CurrentState->Start();

	if (!Player || !Player->ActiveProsthesis)
	{
		Log::Print("No active prosthesis found for player");
		return;
	}
	CurrentCombatState = Player->ActiveProsthesis->GetPrimaryAttackState();
}


// Called every frame
void UPlayerStateMachine::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	if (!PLAYER->IsDead)
	{
		if (!CurrentState)
		{
			Log::Print("No CurrentMovementState found in player state machine");
			return;
		}

		//Update current state if movement is not blocked
		if (!Player->MovementBlocked)
			CurrentState->Update(DeltaTime);

		if (!CurrentCombatState)
		{
			Log::Print("No CurrentCombatState found in player state machine");
			return;
		}

		//Update combat state if player character is in combat
		if (Player->InCombat)
			CurrentCombatState->Update(DeltaTime);
	}
}

void UPlayerStateMachine::CombatStateTransition(UProsthesisBaseState* NewCombatState)
{
	if (!NewCombatState)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid CombatState for transition"))
		return;
	}
	
	CurrentCombatState->End();
	CurrentCombatState = NewCombatState;
	CurrentCombatState->Start();
}

void UPlayerStateMachine::DeactivateCombatState() const
{
	DisableCombatState();
	CurrentCombatState->End();
}

void UPlayerStateMachine::DisableCombatState() const
{
	Player->InCombat = false;
}

void UPlayerStateMachine::EnableCombatState() const
{
	Player->InCombat = true;
}

void UPlayerStateMachine::DeactivateMovementState() const
{
	DisableMovementState();
	CurrentState->End();
}

void UPlayerStateMachine::DisableMovementState() const
{
	Player->MovementBlocked = true;
}

void UPlayerStateMachine::EnableMovementState() const
{
	Player->MovementBlocked = false;
}
