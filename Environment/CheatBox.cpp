// Fill out your copyright notice in the Description page of Project Settings.


#include "CheatBox.h"

#include "Duskborn/DuskbornGameModeBase.h"
#include "Duskborn/Helper/Log.h"
#include "Duskborn/Management/WorldManager.h"
#include "Duskborn/UpgradeSystem/TriggerManager.h"

ACheatBox::ACheatBox() : AInteractable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	LootBox = CreateDefaultSubobject<UStaticMeshComponent>("LootBox");
	LootBox->SetupAttachment(RootComponent);
}

void ACheatBox::Interact()
{
	Super::Interact();

	if (!GetWorld())
	{
		Log::Print("World not found");
		return;
	}

	//Alternate between effect and trigger
	if (ITEM_FACTORY->GetTriggerNext)
	{
		if (TRIGGER_MANAGER->TriggerPartsList.IsEmpty())
		{
			Log::Print("No TriggerParts in TriggerManager");
			return;
		}

		//Get random trigger from trigger manager
		const int32 PartIndex = FMath::RandRange(0, TRIGGER_MANAGER->TriggerPartsList.Num() - 1);

		//Duplicate it and add to player character inventory
		PLAYER->AddTriggerPart(ITEM_FACTORY->DuplicatePart<UTriggerPart>(TRIGGER_MANAGER->TriggerPartsList[PartIndex]));

		ITEM_FACTORY->GetTriggerNext = false;
	}
	else
	{
		if (ITEM_FACTORY->EffectPartsList.IsEmpty())
		{
			Log::Print("No EffectsParts in ItemFactory");
			return;
		}

		//Get random effect from Item Factory
		const int32 PartIndex = FMath::RandRange(0, ITEM_FACTORY->EffectPartsList.Num() - 1);

		//Add effect to player inventory
		PLAYER->AddEffectPart(ITEM_FACTORY->DuplicatePart<UEffectPart>(ITEM_FACTORY->EffectPartsList[PartIndex]));
		
		ITEM_FACTORY->GetTriggerNext = true;
	}

	//Add 100 currency
	PLAYER->SetCurrencyAmount(PLAYER->GetCurrencyAmount() + 100.f);
}
