// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TriggerManager.generated.h"

class UOverTimeTrigger;
enum class ETriggerTypes : uint8;
class UItem;
class UTrigger;
class UTriggerPart;

UCLASS()
class DUSKBORN_API ATriggerManager : public AActor
{
	GENERATED_BODY()

	//-----------------------Variables----------------------------------------------------------------------------------  

	//public variables
public:
	/** List of all Trigger which are currently used in the Game*/
	UPROPERTY(BlueprintReadOnly)
	TMap<ETriggerTypes, UTrigger*> TriggerList;
	
	/** List of all Triggers which need to be updated */
	TArray<UTrigger*> TriggerUpdateList;
	
	/** List of all TriggerParts which can be found in the Game */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UTriggerPart*> TriggerPartsList;

	//protected variables
protected:

	//private variables
private:
	//-----------------------Functions----------------------------------------------------------------------------------

	//public functions  
public:
	ATriggerManager();

	/** Add item to subscribers of trigger */
	void AddItem(UItem* Item);
	/** Remove item from trigger subscribbers */
	void RemoveItem(UItem* Item);
	/** Find trigger by type in TriggerList */
	UTrigger* FindTrigger(ETriggerTypes TriggerType);

	/** Notify all subscribers of Trigger. If location is not defined the effect will use the EffectInstigator origin */
	void Notify(AActor* EffectInstigator, ETriggerTypes TriggerType, const FVector* Location = nullptr);

	/** Fill threshold of Trigger. */
	void FillTriggerThreshold(ETriggerTypes TriggerType, float Amount);
	
	void ActivateTrigger(ETriggerTypes TriggerType);
	void DeactivateTrigger(ETriggerTypes TriggerType);

	//protected functions
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	//private functions  
private:
};
