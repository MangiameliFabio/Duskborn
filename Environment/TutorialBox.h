// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "TutorialBox.generated.h"

class UUpgradePart;
class UNiagaraComponent;
class UTriggerPart;
class UEffectPart;

//The tutorial box is a special interactable which is currently used in the tutorial to give a specified upgrade part to the player
UCLASS()
class DUSKBORN_API ATutorialBox : public AInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATutorialBox();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* ItemVFX = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* Root = nullptr;

	/** Choose which UpgradePart should drop. */
	UPROPERTY(EditAnywhere)
	UUpgradePart* UpgradePart = nullptr;

protected:
	//protected variables
	UPROPERTY(BlueprintReadOnly)
	bool AlreadyUsed = false;

public:
	
	virtual void Interact() override;
};