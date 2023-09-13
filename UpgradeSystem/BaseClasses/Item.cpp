// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

#include "Effect.h"


UItem::UItem()
{
}

void UItem::Init(UEffectPart* NewEffect, UTriggerPart* NewTrigger, AEffect* NewEffectActor)
{
	Effect = NewEffect;
	Trigger = NewTrigger;
	EffectActor = NewEffectActor;
	EffectActor->SetItemUsed(this);
}


void UItem::ActivateEffect(AActor* EffectInstigator, const FVector* Location) const
{
	if (EffectInstigator)
		// Set on which Actor the effect should spawn. Most of the time its the player or an enemy character
		EffectActor->SetEffectInstigator(EffectInstigator);

	if (Location)
		//If a special start location is defined use this position
		EffectActor->SetEffectStartLocation(*Location);
	else
		EffectActor->SetEffectStartLocation(EffectInstigator->GetActorLocation());

	EffectActor->OnTriggerActivated();
}

void UItem::DeactivateEffect() const
{
	EffectActor->OnTriggerDeactivated();
}
