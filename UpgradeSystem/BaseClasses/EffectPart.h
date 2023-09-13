// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UpgradePart.h"
#include "EffectPart.generated.h"

class AEffect;
/**
 * EffectParts need an additional class of the EffectActor which should be spawned
 */
UCLASS()
class DUSKBORN_API UEffectPart : public UUpgradePart
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AEffect> EffectClass = nullptr;
};
