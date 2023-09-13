#include "Trigger.h"

#include "Item.h"
#include "TriggerPart.h"
#include "Duskborn/Management/WorldManager.h"

void UTrigger::NotifySubscribers(AActor* EffectInstigator, const FVector* Location)
{
	if (!Subscriber.IsEmpty())
	{
		for (int i = 0; i < Subscriber.Num(); ++i)
		{
			if (Subscriber[i]->IsEquipped)
			{
				Subscriber[i]->ActivateEffect(EffectInstigator, Location);
			}
		}
	}
}

void UTrigger::DeactivateSubscribers()
{
	if (!Subscriber.IsEmpty())
	{
		for (int i = 0; i < Subscriber.Num(); ++i)
		{
			if (Subscriber[i]->IsEquipped)
			{
				Subscriber[i]->DeactivateEffect();
			}
		}
	}
}

void UTrigger::Add(UItem* Item)
{
	Subscriber.Add(Item);
}

void UTrigger::Remove(UItem* Item)
{
	Subscriber.Remove(Item);
}

void UTrigger::Update(float DeltaTime)
{
}

void UTrigger::Activate()
{
	Active = true;
}

void UTrigger::Deactivate()
{
	Active = false;
	CurrentThreshold = 0.f;
	CurrentTriggerFrequency = 0.f;
	CurrentTimeFrame = 0.f;

	DeactivateSubscribers();
}

void UTrigger::FillThreshold(float Amount)
{
	if (!Active)
	{
		CurrentThreshold += Amount;
		if (CurrentThreshold >= ThreshHold)
		{
			if (!ShouldUpdate)
			{
				const FVector Location = PLAYER->GetActorLocation();
				NotifySubscribers(PLAYER, &Location);
			}
			else
			{
				CurrentTriggerFrequency = TriggerFrequency;
				Activate();
			}
		}
	}
}

void UTrigger::Init(const UTriggerPart* TriggerPart)
{
	Type = TriggerPart->Type;
	ShouldUpdate = TriggerPart->ShouldUpdate;
	EffectScale = TriggerPart->EffectScale;
	TriggerFrequency = TriggerPart->TriggerFrequency;
	ThreshHold = TriggerPart->ThreshHold;
	ThresholdTimeFrame = TriggerPart->TimeFrame;
}
