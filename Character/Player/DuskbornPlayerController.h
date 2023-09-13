// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DuskbornPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DUSKBORN_API ADuskbornPlayerController : public APlayerController
{
	GENERATED_BODY()

	ADuskbornPlayerController():APlayerController(){}
	
	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
};