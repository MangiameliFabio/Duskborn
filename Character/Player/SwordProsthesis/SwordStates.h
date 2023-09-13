// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Duskborn/Character/BaseClasses/State.h"
#include "SwordStates.generated.h"


class ASkewerDamage;
class ASlashDamage;
class ADamageObject;
//Sword First Slash State
//----------------------------------------------------------------------------------------------------------------------
UCLASS()
class DUSKBORN_API USwordSlash : public UProsthesisBaseState
{
	GENERATED_BODY()

public:
	virtual void Start() override;
	virtual void End() override;
	virtual void Update(float DeltaTime) override;
	virtual bool HandleAction(EInputs Input, EInputEvent Event) override;
	virtual bool HandleAxisInput(EInputs Input, float AxisValue) override;

	/** Triggered in ABP_Player by slash animations */
	virtual void AttackNotify() override;

	UPROPERTY()
	ASlashDamage* SlashDamageObject = nullptr;
};

//Skewer State
//----------------------------------------------------------------------------------------------------------------------
UCLASS()
class DUSKBORN_API USwordSkewer : public UProsthesisBaseState
{
	GENERATED_BODY()

public:
	virtual void Start() override;
	virtual void End() override;
	virtual void Update(float DeltaTime) override;
	/** Triggered in ABP_Player by skewer animations */
	virtual void AttackNotify() override;

	FVector AimVector = FVector::Zero();
	bool MoveCharacter = false;

	UPROPERTY()
	ASkewerDamage* SkewerDamageObject = nullptr;
};
