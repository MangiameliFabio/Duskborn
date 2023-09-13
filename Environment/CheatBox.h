// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "GameFramework/Actor.h"
#include "CheatBox.generated.h"

class USphereComponent;

//This interactable is only created for development. It will give you alternate an effect and a trigger every time you interact with it
//Player character will also receive currency every time

UCLASS()
class DUSKBORN_API ACheatBox : public AInteractable
{
	GENERATED_BODY()

	//-----------------------Variables----------------------------------------------------------------------------------  

	//public variables
public:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* LootBox;

	//protected variables
protected:
	//private variables
private:
	//-----------------------Functions----------------------------------------------------------------------------------

	//public functions  
public:
	ACheatBox();

	UFUNCTION(BlueprintCallable)
	virtual void Interact() override;
	
	//protected functions
protected:
	//private functions  
private:
};
