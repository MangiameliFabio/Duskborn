// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.generated.h"


/** Object which the player character can interact with */
UCLASS()
class DUSKBORN_API AInteractable : public AActor
{
	GENERATED_BODY()
 
	//-----------------------Variables----------------------------------------------------------------------------------  
 
	//public variables
	public:
 
	//protected variables
	protected:
 
	//private variables
	private:
 
	//-----------------------Functions----------------------------------------------------------------------------------
     
	//public functions  
	public:
	AInteractable();

	/** Event which can be implemented in C++ */
	virtual void Interact();

	/** Event to implement in Blueprints */
	UFUNCTION(BlueprintImplementableEvent)
	void InteractBlueprint();

	//protected functions
	protected:
     
	//private functions  
	private:
	
};
