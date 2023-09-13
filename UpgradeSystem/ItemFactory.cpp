// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemFactory.h"

#include "TriggerManager.h"
#include "Duskborn/DuskbornGameModeBase.h"
#include "Duskborn/Helper/Log.h"
#include "Duskborn/Management/WorldManager.h"
#include "BaseClasses/Effect.h"
#include "BaseClasses/EffectPart.h"
#include "BaseClasses/Item.h"
#include "BaseClasses/Trigger.h"
#include "BaseClasses/TriggerPart.h"
#include "Duskborn/Enums.h"


// Sets default values
AItemFactory::AItemFactory()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

UItem* AItemFactory::CreateItem(UEffectPart* EffectPart, UTriggerPart* TriggerPart)
{
	if (!GetWorld())
	{
		Log::Print("World not found");
		return nullptr;
	}

	//Create new Item
	UItem* NewItem = NewObject<UItem>();

	if (EffectPart == nullptr || TriggerPart == nullptr)
	{
		Log::Print("Effect or Trigger null");
		return nullptr;
	}

	//Create reference for Effect
	AEffect* Effect = nullptr;

	if (TRIGGER_MANAGER)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = PLAYER;

		const FVector Location = FVector::Zero();
		const FRotator Rotation = FRotator::ZeroRotator;

		//Spawn Effect in World. The class to spawned is defined in the EffectPart
		Effect = GetWorld()->SpawnActor<AEffect>(EffectPart->EffectClass, Location, Rotation, SpawnParams);
		//Initialize the effect with the scale defined in the TriggerPart. This is used to Balance Effect/Trigger combinations
		Effect->Init(TRIGGER_MANAGER->FindTrigger(TriggerPart->Type)->EffectScale);
	}

	if (Effect)
	{
		//Initialize the Item with EffectPart, TriggerPart and the actual EffectActor which was created previously
		NewItem->Init(EffectPart, TriggerPart, Effect);
	}
	else
	{
		Log::Print("No effect found");
		return nullptr;
	}

	if (TRIGGER_MANAGER)
	{
		//Add the item to the subscriber list of used trigger
		TRIGGER_MANAGER->AddItem(NewItem);
	}
	else
	{
		Log::Print("No valid Trigger Manager found");
		return nullptr;
	}

	if (PLAYER)
	{
		//Add the item to the player inventory
		PLAYER->AddItem(NewItem);
		return NewItem;
	}

	Log::Print("Item could not be created!");
	return nullptr;
}

void AItemFactory::FillRarityLists()
{
	if (!GetWorld())
	{
		Log::Print("World not found");
		return;
	}

	//Loop through all effects saved in EffectPartList
	for (UEffectPart* Effect : EffectPartsList)
	{
		//Filter Effects by rarity
		switch (Effect->Rarity)
		{
			//Fill the rarity lists for later use in the LootBoxes
		case ERarity::MARK_I: MarkOneEffectList.Add(Effect);
			break;
		case ERarity::MARK_II: MarkTwoEffectList.Add(Effect);
			break;
		case ERarity::MARK_III: MarkThreeEffectList.Add(Effect);
			break;
		default: break;
		}
	}

	if (!TRIGGER_MANAGER)
	{
		Log::Print("Trigger Manager not found");
		return;
	}

	for (UTriggerPart* Trigger : TRIGGER_MANAGER->TriggerPartsList)
	{
		//Filter Trigger by rarity
		switch (Trigger->Rarity)
		{
			//Fill the rarity lists for later use in the LootBoxes
		case ERarity::MARK_I: MarkOneTriggerList.Add(Trigger);
			break;
		case ERarity::MARK_II: MarkTwoTriggerList.Add(Trigger);
			break;
		case ERarity::MARK_III: MarkThreeTriggerList.Add(Trigger);
			break;
		default: break;
		}
	}
}

void AItemFactory::BeginPlay()
{
	Super::BeginPlay();

	for (UEffectPart* EffectPart : EffectPartsList)
	{
		EffectPart->Init();
	}

	FillRarityLists();
}
