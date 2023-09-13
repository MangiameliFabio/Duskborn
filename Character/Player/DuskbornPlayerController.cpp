// Fill out your copyright notice in the Description page of Project Settings.


#include "Duskborn/Character/Player/DuskbornPlayerController.h"

#include "PlayerCharacter.h"
#include "Duskborn/Helper/Log.h"

void ADuskbornPlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PreProcessInput(DeltaTime, bGamePaused);

	//Used to get dash direction. Last input direction of movement component can't be used while swinging.
	//Because inputs while swinging with grappling hook are not handled by movement component.
	if (const APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn()))
	{
		PlayerCharacter->InputManager->UpdateLastInputDirection();
	}
	else
	{
		Log::Print("Cast to Player character failed in Player Controller!");
	}
}