// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UObject/Object.h"
#include "Item.generated.h"

class UTriggerPart;
class UEffectPart;
class AEffect;
class UUpgradePart;
/**
 * 
 */
UCLASS(Blueprintable)
class DUSKBORN_API UItem : public UObject
{
	GENERATED_BODY()

	//-----------------------Variables----------------------------------------------------------------------------------  

	//public variables
public:
	/** EffectPart used for Item */
	UPROPERTY(BlueprintReadOnly)
	UEffectPart* Effect = nullptr;

	/** TriggerPart used for Item */
	UPROPERTY(BlueprintReadOnly)
	UTriggerPart* Trigger = nullptr;

	/** Actor which will spawn the effect */
	UPROPERTY(BlueprintReadOnly)
	AEffect* EffectActor = nullptr;

	/** True if the item is currently in use by the player */
	UPROPERTY(BlueprintReadWrite)
	bool IsEquipped = false;

	//protected variables
protected:
	//private variables
private:
	//-----------------------Functions----------------------------------------------------------------------------------

	//public functions  
public:
	UItem();
	void Init(UEffectPart* NewEffect, UTriggerPart* NewTrigger, AEffect* NewEffectActor);

	/** Activate the actual effect of the EffectPart */
	void ActivateEffect(AActor* EffectInstigator, const FVector* Location) const;
	/** Deactivate the actual effect of the EffectPart */
	void DeactivateEffect() const;
	//protected functions
protected:
	//private functions  
private:
};
