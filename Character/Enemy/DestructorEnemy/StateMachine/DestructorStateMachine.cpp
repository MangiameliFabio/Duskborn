// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructorStateMachine.h"

#include "DestructorStates.h"
#include "Duskborn/Character/Enemy/DestructorEnemy/DestructorEnemy.h"


// Sets default values for this component's properties
UDestructorStateMachine::UDestructorStateMachine() : UEnemyStateMachine()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDestructorStateMachine::BeginPlay()
{
	Super::BeginPlay();

	Destructor = Cast<ADestructorEnemy>(GetOwner());

	//Create new destructor state
	IdleState = NewObject<UDestructorIdle>();
	IdleState->Init(this);
	ActiveState = NewObject<UDestructorActive>();
	ActiveState->Init(this);

	//Set idle as default state
	CurrentState = IdleState;
	CurrentState->Start();
}


// Called every frame
void UDestructorStateMachine::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	if (!Destructor->IsDead)
	{
		Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	}
}
