#include "OverTimeTrigger.h"

#include "Duskborn/Management/WorldManager.h"

void UOverTimeTrigger::Update(float DeltaTime)
{
	//Check if active
	if (Active)
	{
		//If the trigger is active. Notify all subscriber periodically
		if (CurrentTriggerFrequency >= TriggerFrequency)
		{
			NotifySubscribers(PLAYER);
			CurrentTriggerFrequency = 0.f;
		}
	}
	
	if (CurrentTriggerFrequency <= TriggerFrequency)
	{
		CurrentTriggerFrequency += DeltaTime;
	}
}
