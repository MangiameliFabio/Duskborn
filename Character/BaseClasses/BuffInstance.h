// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BuffInstance.generated.h"

class ABuff;
enum class EStats : uint8;
/**
 * 
 */
UCLASS(Blueprintable)
class DUSKBORN_API ABuffInstance : public AActor
{
	GENERATED_BODY()

	//-----------------------Variables----------------------------------------------------------------------------------  

	//public variables
public:
	float BuffAmount = 0;
	
	//protected variables
protected:
	//private variables
private:
	UPROPERTY()
	FTimerHandle BuffDurationTimer;

	/** Reference to actual buff */
	UPROPERTY()
	ABuff* BuffParent = nullptr;

	EStats StatToBuff;
	//-----------------------Functions----------------------------------------------------------------------------------

	//public functions  
public:
	ABuffInstance();
	void Init(EStats Stat, ABuff* Parent, float Amount);

	/** Set timer after which the buff instance will delete itself*/
	UFUNCTION(BlueprintCallable)
	void SetBuffTimer(float Duration);
	void OnInstanceDestroyed();
	void CleanUp();

	//protected functions
protected:
	//private functions  
private:
};
