// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "GameFramework/Actor.h"
#include "PartLootBox.generated.h"

class UNiagaraComponent;
class USphereComponent;

UCLASS()
class DUSKBORN_API APartLootBox : public AInteractable
{
	GENERATED_BODY()

	//-----------------------Variables----------------------------------------------------------------------------------  

	//public variables
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* LootBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* ItemVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* Root;

	//protected variables
protected:
	UPROPERTY(BlueprintReadOnly)
	bool AlreadyUsed = false;

	//private variables
private:
	//-----------------------Functions----------------------------------------------------------------------------------

	//public functions  
public:
	APartLootBox();

	UFUNCTION(BlueprintCallable)
	virtual void Interact() override;

	//protected functions
protected:
	//private functions  
private:
};
