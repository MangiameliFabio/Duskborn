// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Duskborn/UpgradeSystem/BaseClasses/TriggerPart.h"
#include "Duskborn/UpgradeSystem/BaseClasses/EffectPart.h"

#include "ItemFactory.generated.h"

class UItem;
struct FUpgradePart;
class AEffect;

UCLASS()
class DUSKBORN_API AItemFactory : public AActor
{
	GENERATED_BODY()

	//-----------------------Variables----------------------------------------------------------------------------------  

	//public variables
public:
	/** List of all chainable EffectParts in the game */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Duskborn")
	TArray<UEffectPart*> EffectPartsList;

	//Rarity Lists which are used in the LootBoxes
	UPROPERTY()
	TArray<UEffectPart*> MarkOneEffectList;
	UPROPERTY()
	TArray<UEffectPart*> MarkTwoEffectList;
	UPROPERTY()
	TArray<UEffectPart*> MarkThreeEffectList;

	UPROPERTY()
	TArray<UTriggerPart*> MarkOneTriggerList;
	UPROPERTY()
	TArray<UTriggerPart*> MarkTwoTriggerList;
	UPROPERTY()
	TArray<UTriggerPart*> MarkThreeTriggerList;

	//Only for cheat box
	bool GetTriggerNext = false;

	/** Chance to receive a TriggerPart in the next loot box. Default is 50%.
	* The chance gets smaller if a TriggerPart was received in the last LootBox or bigger if it was an EffectPart */
	int TriggerChance = 50;

	//protected variables
protected:
	//private variables
private:
	//-----------------------Functions----------------------------------------------------------------------------------

	//public functions  
public:
	AItemFactory();

	/** @return NewItem which was combined out of an EffectPart and a TriggerPart*/
	UFUNCTION(BlueprintCallable)
	UItem* CreateItem(UEffectPart* EffectPart, UTriggerPart* TriggerPart);

	// Loop through all entries of EffectPartsList and TriggerPartsList. Fill add parts to rarity lists
	void FillRarityLists();

	/** Template which can be used to create a duplicate of an UClass. Used to create duplicates of the UpgradeParts
	 * stored in EffectPartsList or TriggerPartsList in the TriggerManager*/
	template <typename T>
	T* DuplicatePart(const T* Part) const
	{
		//Get class from the EffectPart which should be duplicated
		const UClass* PartClass = Part->GetClass();

		//Get class name from the EffectPart which should be duplicated
		const FString BaseObjectName = Part->GetName();

		// Use the original object's name as a base and generate a unique name
		const FString UniqueObjectName = MakeUniqueObjectName(GetTransientPackage(), PartClass, *BaseObjectName).
			ToString();

		//Actual object duplication
		T* NewPart = DuplicateObject<T>(Part, GetTransientPackage(),
		                                FName(*UniqueObjectName));

		// Reset the outer package of the duplicated object
		NewPart->Rename(nullptr, GetTransientPackage(), REN_DontCreateRedirectors | REN_DoNotDirty);

		//Return the new part
		return NewPart;
	}

	//protected functions
protected:
	virtual void BeginPlay() override;

	//private functions  
private:
};
