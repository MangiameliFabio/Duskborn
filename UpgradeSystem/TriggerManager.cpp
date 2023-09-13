// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerManager.h"

#include "BaseClasses/Item.h"
#include "BaseClasses/Trigger.h"
#include "BaseClasses/TriggerPart.h"
#include "Duskborn/Helper/Log.h"
#include "Duskborn/Management/WorldManager.h"
#include "Trigger/OverTimeTrigger.h"


// Sets default values
ATriggerManager::ATriggerManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ATriggerManager::AddItem(UItem* Item)
{
	//Find Trigger in TriggerList and create reference. 
	UTrigger* TriggerRef = FindTrigger(Item->Trigger->Type);
	if (!TriggerRef)
	{
		Log::Print(
			"Can't find Item trigger name in TriggerList. You probably need to set the TriggerSpecification in Trigger Manager");
		return;
	}
	//Add Item to subscribers of Trigger
	TriggerRef->Add(Item);
}

void ATriggerManager::RemoveItem(UItem* Item)
{
	//Find Trigger in TriggerList and create reference. 
	UTrigger* TriggerRef = FindTrigger(Item->Trigger->Type);
	if (!TriggerRef)
	{
		Log::Print(
			"Can't find Item trigger name in TriggerList. You probably need to set the TriggerSpecification in Trigger Manager");
		return;
	}
	//Remove Item from Trigger subscribers
	TriggerRef->Remove(Item);
}

UTrigger* ATriggerManager::FindTrigger(ETriggerTypes TriggerType)
{
	if (TriggerList.IsEmpty())
	{
		Log::Print("Trigger List is Empty. In FindTrigger()");
		return nullptr;
	}

	if (UTrigger** TriggerRef = TriggerList.Find(TriggerType))
		return *TriggerRef;

	return nullptr;
}

void ATriggerManager::Notify(AActor* EffectInstigator, ETriggerTypes TriggerType, const FVector* Location)
{
	//Find Trigger in TriggerList and create reference. 
	UTrigger* TriggerRef = FindTrigger(TriggerType);
	if (!TriggerRef)
	{
		Log::Print(
			"Can't find Item trigger name in TriggerList. You probably need to set the TriggerSpecification in Trigger Manager");
		return;
	}
	TriggerRef->NotifySubscribers(EffectInstigator, Location);
}

void ATriggerManager::FillTriggerThreshold(ETriggerTypes TriggerType, float Amount)
{
	//Find Trigger in TriggerList and create reference. 
	UTrigger* TriggerRef = FindTrigger(TriggerType);
	if (!TriggerRef)
	{
		Log::Print(
			"Can't find Item trigger name in TriggerList. You probably need to set the TriggerSpecification in Trigger Manager");
		return;
	}
	TriggerRef->FillThreshold(Amount);
}

void ATriggerManager::ActivateTrigger(ETriggerTypes TriggerType)
{
	//Find Trigger in TriggerList and create reference. 
	UTrigger* TriggerRef = FindTrigger(TriggerType);
	if (!TriggerRef)
	{
		Log::Print("Trigger not found in List");
		return;
	}
	if (!TriggerRef->Active)
	{
		TriggerRef->Activate();
	}
}

void ATriggerManager::DeactivateTrigger(ETriggerTypes TriggerType)
{
	//Find Trigger in TriggerList and create reference. 
	UTrigger* TriggerRef = FindTrigger(TriggerType);
	if (!TriggerRef)
	{
		Log::Print("Trigger not found in List");
		return;
	}
	if (TriggerRef->Active)
	{
		TriggerRef->Deactivate();
	}
}

void ATriggerManager::BeginPlay()
{
	Super::BeginPlay();

	//Each TriggerPart creates a Trigger. Reserve space for the amount of TriggerParts in list
	TriggerList.Reserve(TriggerPartsList.Num());

	//Loop through all TriggerParts
	for (const auto TriggerPart : TriggerPartsList)
	{
		if (!TriggerPart->TriggerClass)
		{
			Log::Print("No Trigger Class selected in specification");
		}
		//Initialize TriggerPart. Set scrap value and energy costs
		TriggerPart->Init();

		//Create Trigger from class defined in TriggerPArt
		UTrigger* TriggerRef = NewObject<UTrigger>(GetTransientPackage(), TriggerPart->TriggerClass);

		if (!TriggerRef)
		{
			Log::Print("Can't create NewTrigger");
			return;
		}
		//Initialize Trigger with variables from TriggerPart
		TriggerRef->Init(TriggerPart);

		//Add new Trigger to TriggerList
		TriggerList.Add(TriggerRef->Type, TriggerRef);

		//If the new Trigger should update, also add it to TriggerUpdateList
		if (TriggerRef->ShouldUpdate)
		{
			TriggerUpdateList.Add(TriggerRef);
		}
	};
}

void ATriggerManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//Update Trigger
	if (!TriggerUpdateList.IsEmpty())
	{
		for (UTrigger* Trigger : TriggerUpdateList)
		{
			check(Trigger);
			Trigger->Update(DeltaSeconds);
		}
	}
}
