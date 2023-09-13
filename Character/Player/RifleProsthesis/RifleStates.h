// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Duskborn/Character/Player/StateMachine/PlayerStates.h"
#include "RifleStates.generated.h"

class ARifleProsthesis;

//Ground Movement State
//----------------------------------------------------------------------------------------------------------------------
UCLASS()
class DUSKBORN_API URifleFireState : public UProsthesisBaseState
{
	GENERATED_BODY()
	
	virtual void Start() override;
	virtual void End() override;
	virtual void Update(float DeltaTime) override;
	virtual bool HandleAction(EInputs Input, EInputEvent Event) override;
	virtual bool HandleAxisInput(EInputs Input, float AxisValue) override;
};

//Killer Instinct State
//----------------------------------------------------------------------------------------------------------------------
UCLASS()
class DUSKBORN_API UKillerInstinctState : public UProsthesisBaseState
{
	GENERATED_BODY()
	
	virtual void Start() override;
	virtual void End() override;
	virtual void Update(float DeltaTime) override;
	virtual bool HandleAction(EInputs Input, EInputEvent Event) override;
	virtual bool HandleAxisInput(EInputs Input, float AxisValue) override;
	virtual void AttackNotify() override;
};