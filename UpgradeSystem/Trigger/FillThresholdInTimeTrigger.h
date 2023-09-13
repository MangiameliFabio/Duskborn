// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OverTimeTrigger.h"
#include "FillThresholdInTimeTrigger.generated.h"

/**
 * 
 */
UCLASS()
class DUSKBORN_API UFillThresholdInTimeTrigger : public UTrigger
{
	GENERATED_BODY()

	virtual void FillThreshold(float Amount) override;

	virtual void Update(float DeltaSeconds) override;
};
