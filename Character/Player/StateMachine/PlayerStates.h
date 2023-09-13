// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Duskborn/Character/BaseClasses/State.h"
#include "PlayerStates.generated.h"

//Ground Movement State
//----------------------------------------------------------------------------------------------------------------------

UCLASS()
class DUSKBORN_API UPlayerGroundMovement : public UPlayerBaseState
{
	GENERATED_BODY()

	virtual void Start() override;
	virtual void End() override;
	virtual void Update(float DeltaTime) override;
	virtual bool HandleAction(EInputs Input, EInputEvent Event) override;
	virtual bool HandleAxisInput(EInputs Input, float AxisValue) override;
};

//Grappling State
//----------------------------------------------------------------------------------------------------------------------

UCLASS()
class DUSKBORN_API UPlayerGrappling : public UPlayerBaseState
{
	GENERATED_BODY()

	virtual void Start() override;
	virtual void End() override;
	virtual void Update(float DeltaTime) override;
	virtual bool HandleAction(EInputs Input, EInputEvent Event) override;
	virtual bool HandleAxisInput(EInputs Input, float AxisValue) override;

	FVector LastValidLocation;

	float RopeTolerance = 10.f;
};

//Dash State
//----------------------------------------------------------------------------------------------------------------------

UCLASS()
class DUSKBORN_API UPlayerDash : public UPlayerBaseState
{
	GENERATED_BODY()

	virtual void Start() override;
	virtual void End() override;
	virtual void Update(float DeltaTime) override;
	virtual bool HandleAction(EInputs Input, EInputEvent Event) override;
	virtual bool HandleAxisInput(EInputs Input, float AxisValue) override;
};

//GrapplingDash State
//----------------------------------------------------------------------------------------------------------------------

UCLASS()
class DUSKBORN_API UPlayerGrapplingDash : public UPlayerBaseState
{
	GENERATED_BODY()

	virtual void Start() override;
	virtual void End() override;
	virtual void Update(float DeltaTime) override;
	virtual bool HandleAction(EInputs Input, EInputEvent Event) override;
	virtual bool HandleAxisInput(EInputs Input, float AxisValue) override;

	FVector VelocityBeforeDash = FVector::Zero();
};

//JetPack State
//----------------------------------------------------------------------------------------------------------------------

UCLASS()

class DUSKBORN_API UPlayerJetPack : public UPlayerBaseState
{
	GENERATED_BODY()

	virtual void Start() override;
	virtual void End() override;
	virtual void Update(float DeltaTime) override;
	virtual bool HandleAction(EInputs Input, EInputEvent Event) override;
	virtual bool HandleAxisInput(EInputs Input, float AxisValue) override;
};


//Sprint State
//----------------------------------------------------------------------------------------------------------------------

UCLASS()

class DUSKBORN_API UPlayerSprintState : public UPlayerBaseState
{
	GENERATED_BODY()

	virtual void Start() override;
	virtual void End() override;
	virtual void Update(float DeltaTime) override;
	virtual bool HandleAction(EInputs Input, EInputEvent Event) override;
	virtual bool HandleAxisInput(EInputs Input, float AxisValue) override;
};
