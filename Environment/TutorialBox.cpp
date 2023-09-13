// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialBox.h"

#include "CheatTable.h"
#include "NiagaraComponent.h"

#include "Duskborn/DuskbornGameModeBase.h"
#include "Duskborn/Helper/Log.h"
#include "Duskborn/Management/WorldManager.h"
#include "Duskborn/UpgradeSystem/BaseClasses/EffectPart.h"
#include "Duskborn/UpgradeSystem/BaseClasses/TriggerPart.h"


// Sets default values
ATutorialBox::ATutorialBox() : AInteractable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	ItemVFX = CreateDefaultSubobject<UNiagaraComponent>("ItemVFX");
	ItemVFX->SetupAttachment(Root);
}

void ATutorialBox::Interact()
{
	Super::Interact();

	if (!AlreadyUsed)
	{
		if (!GetWorld())
		{
			Log::Print("World not found");
			return;
		}
		if (!ITEM_FACTORY)
		{
			Log::Print("Item factory not found");
			return;
		}

		if (UpgradePart)
		{
			Log::Print("Please select a UpgradePart class in BP_TutorialBox");
			return;
		}
		//Check if Upgrade part is EffectPart and add to player character inventory. Also give the amount of currency needed to use the part for crafting
		if (UEffectPart* EffectPart = Cast<UEffectPart>(UpgradePart))
		{
			PLAYER->AddEffectPart(ITEM_FACTORY->DuplicatePart(EffectPart));
			PLAYER->AddCurrency(UpgradePart->ScrapValue * 2);
		}
		//Check if Upgrade part is TriggerPart and add to player character inventory. Also give the amount of currency needed to use the part for crafting
		if (UTriggerPart* TriggerPart = Cast<UTriggerPart>(UpgradePart))
		{
			PLAYER->AddTriggerPart(ITEM_FACTORY->DuplicatePart<UTriggerPart>(TriggerPart));
			PLAYER->AddCurrency(TriggerPart->ScrapValue * 2);
		}
		AlreadyUsed = true;
	}
}
