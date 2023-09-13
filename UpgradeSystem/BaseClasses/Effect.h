// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Duskborn/Enums.h"
#include "Duskborn/Management/WorldManager.h"
#include "GameFramework/Actor.h"
#include "Effect.generated.h"

class WorldManager;
class AItemFactory;

UCLASS()
class DUSKBORN_API AEffect : public AActor
{
	GENERATED_BODY()

	//-----------------------Variables----------------------------------------------------------------------------------  

	//public variables
public:
	UPROPERTY(BlueprintReadOnly)
	UItem* ItemUsed = nullptr;

	/** Multiplier which will change how efficient the effect is. Used for balancing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TriggerScale = 0.f;

	//protected variables
protected:
	//private variables
private:

	/** Instigator which caused the effect. */
	UPROPERTY()
	AActor* EffectInstigator = nullptr;
	/** Location where the effect should start */
	FVector EffectStartLocation = FVector::Zero();

public:
	//-----------------------Functions----------------------------------------------------------------------------------
	//public functions  
	AEffect();
	void Init(float NewTriggerScale);

	/** Event which is called when the trigger activates the effect*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnTriggerActivated();
	/** Event which is called when the trigger deactivates the effect. Currently not used in Blueprints */
	UFUNCTION(BlueprintImplementableEvent)
	void OnTriggerDeactivated();

	/** Event when effect is unequipped. Especially used for permanent stat upgrades. Will remove the given stats from the player character */
	UFUNCTION(BlueprintImplementableEvent)
	void OnUnequipped();
	/** Event when effect is equipped. Especially used for permanent stat upgrades. Will add the stat buffs already earned to the player character */
	UFUNCTION(BlueprintImplementableEvent)
	void OnEquipped();

	UFUNCTION(BlueprintCallable)
	AActor* GetEffectInstigator();
	UFUNCTION(BlueprintCallable)
	void SetEffectInstigator(AActor* NewEffectTarget);
	UFUNCTION(BlueprintCallable)
	FVector GetEffectStartLocation() const;
	UFUNCTION(BlueprintCallable)
	void SetEffectStartLocation(const FVector& NewEffectStartLocation);

	void SetItemUsed(UItem* NewItemUsed);


	//protected functions
protected:
	//private functions  
private:
};
