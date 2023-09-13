// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UpgradePart.h"
#include "Duskborn/Enums.h"
#include "UObject/Object.h"
#include "Engine/DataAsset.h"
#include "TriggerPart.generated.h"

class UTrigger;
enum class ETriggerTypes : uint8;
/**
 * 
 */
UCLASS()
class DUSKBORN_API UTriggerPart final : public UUpgradePart
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETriggerTypes Type = ETriggerTypes::DEFAULT;
	
	/** Scalar for Effect attributes */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float EffectScale = 1.f;

	/** How often should the trigger activate an effect */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TriggerFrequency = 1.f;

	/** TimeFrame in which the effect can be triggered. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TimeFrame = 1.f;

	/** Threshold value which needs to be reached to trigger an Effect. This could be the amount of HP needed to trigger "While HP is below {Threshold}. Or After {Threshold} amount of time*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ThreshHold = 0.f;

	/** Trigger which are used periodically, like WhileStanding need to be updated*/
	UPROPERTY(EditAnywhere)
	bool ShouldUpdate = false;

	/** Class for Trigger */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UTrigger> TriggerClass = nullptr;
};
