// Fill out your copyright notice in the Description page of Project Settings.


#include "CheatTable.h"

#include "Duskborn/DuskbornGameModeBase.h"
#include "Duskborn/Helper/Log.h"
#include "Duskborn/Management/WorldManager.h"


// Sets default values
ACheatTable::ACheatTable() : AInteractable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ACheatTable::BeginPlay()
{
	Super::BeginPlay();
}

void ACheatTable::Interact()
{
	Super::Interact();

	if (!GetWorld())
	{
		Log::Print("World not found");
		return;
	}
	if (!EffectPart || !TriggerPart || !ITEM_FACTORY)
	{
		Log::Print("Wenn du schon cheatest machs wenigstens RICHTIG!!");
		return;
	}

	//Create item with defined classes in item factory
	UItem* NewItem = ITEM_FACTORY->CreateItem(EffectPart, TriggerPart);

	//Equip player with item
	PLAYER->EquipItem(NewItem);
}
