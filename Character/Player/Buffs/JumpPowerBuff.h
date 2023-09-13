// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Duskborn/Character/BaseClasses/Buff.h"
#include "JumpPowerBuff.generated.h"

UCLASS()
class DUSKBORN_API AJumpPowerBuff : public ABuff
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AJumpPowerBuff();

	virtual void AddInstance(ABuffInstance* Instance) override;
	virtual void InstanceRemoved(ABuffInstance* Instance) override;
};
