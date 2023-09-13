// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable.h"


// Sets default values
AInteractable::AInteractable() : AActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AInteractable::Interact()
{
	//Fire blueprint event to always trigger both events
	InteractBlueprint();
}
