// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OverTimeTrigger.h"
#include "Duskborn/UpgradeSystem/BaseClasses/Trigger.h"
#include "AllTheTime.generated.h"

/**
 * 
 */
UCLASS()
class DUSKBORN_API UAllTheTime : public UOverTimeTrigger
{
	GENERATED_BODY()

	virtual void Init(const UTriggerPart* TriggerSpecification) override;
};
