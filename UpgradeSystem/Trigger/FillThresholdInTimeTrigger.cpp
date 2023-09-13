// Fill out your copyright notice in the Description page of Project Settings.


#include "FillThresholdInTimeTrigger.h"

#include "Duskborn/Helper/Log.h"
#include "Duskborn/Management/WorldManager.h"

void UFillThresholdInTimeTrigger::FillThreshold(float Amount)
{
	//Check if threshold is reached
	if (CurrentThreshold >= ThreshHold)
	{
		//Reset Threshold
		CurrentThreshold = 0.f;

		//Notify all items subscribed to trigger
		NotifySubscribers(PLAYER);

		//Reset TimeFrame
		CurrentTimeFrame = 0.f;
	}
	else
	{
		CurrentThreshold += Amount;
	}
}

void UFillThresholdInTimeTrigger::Update(float DeltaSeconds)
{
	//Check if the max time passed after which the threshold will reset. This is used for trigger like "XDamageInXAmountOfTime"
	if (CurrentTimeFrame >= ThresholdTimeFrame)
	{
		CurrentThreshold = 0.f;
		CurrentTimeFrame = 0.f;
	}
	else
	{
		CurrentTimeFrame += DeltaSeconds;
	}
}
