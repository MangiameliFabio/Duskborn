// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Duskborn/UpgradeSystem/BaseClasses/Trigger.h"
#include "OverTimeTrigger.generated.h"

/**
 * 
 */
UCLASS()
class DUSKBORN_API UOverTimeTrigger : public UTrigger
{
	GENERATED_BODY()

public:
	virtual void Update(float DeltaTime) override;
};
