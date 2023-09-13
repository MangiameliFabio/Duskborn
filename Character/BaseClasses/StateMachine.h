// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateMachine.generated.h"

class UCombatState;
class APlayerCharacter;
class UState;
class UIdleState;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DUSKBORN_API UStateMachine : public UActorComponent
{
	GENERATED_BODY()

	//-----------------------Variables----------------------------------------------------------------------------------  

	//public variables
public:
	UPROPERTY()
	UState* CurrentState = nullptr;

	//protected variables
protected:
	//private variables
private:
	//-----------------------Functions----------------------------------------------------------------------------------

	//public functions  
public:
	// Sets default values for this component's properties
	UStateMachine();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	virtual void Init(){};

	/** End current state and set to new state. DontEndLastState will prevent ending the current state. This is used for example when the prosthesis swaps but you don't need the state end behaviour */
	void StateTransition(UState* NewState);

	//protected functions
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//private functions  
private:
};
