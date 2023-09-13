// Fill out your copyright notice in the Description page of Project Settings.


#include "PartLootBox.h"

#include "NiagaraComponent.h"
#include "Duskborn/DuskbornGameModeBase.h"
#include "Duskborn/Helper/Log.h"
#include "Duskborn/Management/WorldManager.h"
#include "Duskborn/UpgradeSystem/TriggerManager.h"
#include "Duskborn/UpgradeSystem/BaseClasses/EffectPart.h"

// Sets default values
APartLootBox::APartLootBox() : AInteractable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	ItemVFX = CreateDefaultSubobject<UNiagaraComponent>("ItemVFX");
	ItemVFX->SetupAttachment(Root);
}

void APartLootBox::Interact()
{
	Super::Interact();

	//Interact if not already interacted with
	if (!AlreadyUsed)
	{
		//Get random value to calculate the chance if a trigger or effect part should be selected
		const int PartChance = FMath::RandRange(0, 100);

		//Chance to choose which part rarity will drop
		const int RandRarityChance = FMath::RandRange(0, 100);

		if (!GetWorld())
		{
			Log::Print("World not found");
			return;
		}

		//Chance if Part will be a Trigger or Effect
		if (PartChance <= ITEM_FACTORY->TriggerChance)
		{
			//Make Chance smaller to have a smaller chance to receive a Trigger next time;
			ITEM_FACTORY->TriggerChance /= 2;

			if (TRIGGER_MANAGER->TriggerPartsList.IsEmpty())
			{
				Log::Print("No TriggerParts in TriggerManager");
				return;
			}

			UTriggerPart* NewPart;

			//20% Chance to get an MarkIII Trigger
			if (RandRarityChance <= 20)
			{
				const int32 PartIndex = FMath::RandRange(0, ITEM_FACTORY->MarkThreeTriggerList.Num() - 1);
				NewPart = ITEM_FACTORY->DuplicatePart<UTriggerPart>(ITEM_FACTORY->MarkThreeTriggerList[PartIndex]);
			}
			//30% Chance to get an MarkII Trigger
			else if (RandRarityChance <= 50)
			{
				const int32 PartIndex = FMath::RandRange(0, ITEM_FACTORY->MarkTwoTriggerList.Num() - 1);
				NewPart = ITEM_FACTORY->DuplicatePart<UTriggerPart>(ITEM_FACTORY->MarkTwoTriggerList[PartIndex]);
			}
			//50% Chance to get an MarkI Trigger
			else
			{
				const int32 PartIndex = FMath::RandRange(0, ITEM_FACTORY->MarkOneTriggerList.Num() - 1);
				NewPart = ITEM_FACTORY->DuplicatePart<UTriggerPart>(ITEM_FACTORY->MarkOneTriggerList[PartIndex]);
			}
			if (!NewPart)
			{
				Log::Print("Can't create new Trigger in LootBox");
				return;
			}

			//Add Trigger to player character inventory
			PLAYER->AddTriggerPart(NewPart);
		}
		else
		{
			//Make Chance smaller to receive a Effect next time;
			ITEM_FACTORY->TriggerChance *= 2;

			if (ITEM_FACTORY->EffectPartsList.IsEmpty())
			{
				Log::Print("No EffectParts in ItemFactory");
				return;
			}

			UEffectPart* NewPart;

			//10% Chance to get an MarkIII Effect
			if (RandRarityChance <= 20)
			{
				const int32 PartIndex = FMath::RandRange(0, ITEM_FACTORY->MarkThreeEffectList.Num() - 1);
				NewPart = ITEM_FACTORY->DuplicatePart(ITEM_FACTORY->MarkThreeEffectList[PartIndex]);
			}
			//25% Chance to get an MarkII Effect
			else if (RandRarityChance <= 50)
			{
				const int32 PartIndex = FMath::RandRange(0, ITEM_FACTORY->MarkTwoEffectList.Num() - 1);
				NewPart = ITEM_FACTORY->DuplicatePart(ITEM_FACTORY->MarkTwoEffectList[PartIndex]);
			}
			//50% Chance to get an MarkII Effect
			else
			{
				const int32 PartIndex = FMath::RandRange(0, ITEM_FACTORY->MarkOneEffectList.Num() - 1);
				NewPart = ITEM_FACTORY->DuplicatePart(ITEM_FACTORY->MarkOneEffectList[PartIndex]);
			}
			if (!NewPart)
			{
				Log::Print("Can't create new Effect in LootBox");
				return;
			}
			PLAYER->AddEffectPart(NewPart);
		}

		AlreadyUsed = true;
	}
}
