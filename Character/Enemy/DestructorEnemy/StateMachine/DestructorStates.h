// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Duskborn/Character/BaseClasses/State.h"
#include "DestructorStates.generated.h"

//Idle State
//----------------------------------------------------------------------------------------------------------------------
UCLASS()
class DUSKBORN_API UDestructorIdle : public UState
{
	GENERATED_BODY()

	virtual void Start() override;

	/** Check if player is in range for attack*/
	void PlayerInRange() const;
};

//Active State
//----------------------------------------------------------------------------------------------------------------------
UCLASS()
class DUSKBORN_API UDestructorActive : public UState
{
	GENERATED_BODY()

	virtual void Start() override;
	virtual void End() override;

	/* Destructor active update routine: Charge->Attack->Reload->RangeCheck->Repeat/Idle**/
	virtual void Update(float DeltaTime) override;

};