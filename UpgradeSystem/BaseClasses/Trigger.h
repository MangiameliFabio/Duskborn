// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Duskborn/Enums.h"
#include "Trigger.generated.h"

class UItem;
class UTriggerPart;

UCLASS()
class DUSKBORN_API UTrigger : public UObject
{
	GENERATED_BODY()

public:
	/** Array with all items which subscribed to this trigger */
	TArray<UItem*> Subscriber;

	/** Scalar for Effect attributes */
	float EffectScale = 1.f;

	/** How often should the trigger activate an effect */
	float TriggerFrequency = 1.f;
	float CurrentTriggerFrequency = 1.f;

	/** Threshold value which needs to be reached to trigger an Effect. This could be the amount of HP needed to trigger "While HP is below {Threshold}. Or After {Threshold} amount of time*/
	float ThreshHold = 0.f;
	float CurrentThreshold = 0.f;

	/** TimeFrame in which the threshold needs to be reached to activate the trigger */
	float ThresholdTimeFrame = 1.f;
	float CurrentTimeFrame = 0.f;

	/** Type of the Trigger. New types needs to be added in enum ETriggerTypes */
	ETriggerTypes Type = ETriggerTypes::DEFAULT;

	/** Bool which keeps track if the Trigger is currently active. E.g trigger which fill a threshold are active as soon as the threshold is reached */
	bool Active = false;

	/** Prevent Trigger which are currently not in use by the player to update*/
	bool ShouldUpdate = false;

	virtual void Init(const UTriggerPart* TriggerPart);

	/** Notify all Items which have subscribed to this trigger. Items will activate their effect */
	virtual void NotifySubscribers(AActor* EffectInstigator, const FVector* Location = nullptr);
	/** Notify all Items which have subscribed to this trigger. Items will deactivate their effect */
	virtual void DeactivateSubscribers();
	/** Fill threshold of the Trigger. if Threshold is reached activate the the Trigger */
	virtual void FillThreshold(float Amount);
	/** Add item to subscriber list */
	virtual void Add(UItem* Item);
	/** Remove item from subscriber list */
	virtual void Remove(UItem* Item);

	virtual void Update(float DeltaTime);

	/** Some Trigger will activate the effect periodically, like WhileStanding or AllTheTime. That is why they need to be activated. Trigger which only
	 * trigger once, like OnKill will directly notify their subscriber and don't use Activate() */
	void Activate();

	/** Deactivate trigger and reset variables */
	void Deactivate();
};
