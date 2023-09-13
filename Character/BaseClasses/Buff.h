// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Buff.generated.h"

class ABuffInstance;
class UNiagaraComponent;

UCLASS()
class DUSKBORN_API ABuff : public AActor
{
	GENERATED_BODY()

	//-----------------------Variables----------------------------------------------------------------------------------  

	//public variables
public:
	//protected variables
protected:
	UPROPERTY(EditAnywhere)
	USceneComponent* Root;
	UPROPERTY(EditAnywhere)
	UNiagaraComponent* BuffNiagara;

	//private variables
private:
	TArray<ABuffInstance*> BuffInstances;
	int InstanceCount = 0;

	//-----------------------Functions----------------------------------------------------------------------------------

	//public functions  
public:
	ABuff();

	/** Add an instance of the buff */
	virtual void AddInstance(ABuffInstance* Instance);
	/** Decrease instance count when one is removed. Instance is used in child implementation */
	virtual void InstanceRemoved(ABuffInstance* Instance);

	void CleanUp();
	//protected functions
protected:
	//private functions  
private:
};
