// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "GameFramework/Actor.h"
#include "CheatTable.generated.h"

class UTriggerPart;
class UEffectPart;
UCLASS()

//Only for development. Will give on interaction a specified trigger and effect
class DUSKBORN_API ACheatTable : public AInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACheatTable();

	UPROPERTY(EditAnywhere)
	UEffectPart* EffectPart = nullptr;
	UPROPERTY(EditAnywhere)
	UTriggerPart* TriggerPart = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	
	virtual void Interact() override;
};
