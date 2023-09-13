// Fill out your copyright notice in the Description page of Project Settings.


#include "AllTheTime.h"

void UAllTheTime::Init(const UTriggerPart* TriggerSpecification)
{
	Super::Init(TriggerSpecification);

	// The AllTheTime Trigger needs to be activated instantly when being created
	Activate();
}
