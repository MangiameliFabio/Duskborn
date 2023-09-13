// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Duskborn/Character/BaseClasses/Buff.h"
#include "Duskborn/Character/BaseClasses/BuffInstance.h"
#include "Duskborn/Management/WorldManager.h"
#include "Components/ActorComponent.h"
#include "BuffManagerComponent.generated.h"

class APullSpeedBuff;
class AMovementSpeedBuff;
class AJumpPowerBuff;
class AHealthRegBuff;
class AAttackDamageBuff;
class ABuffInstance;

enum class EStats : uint8;
class ACritChanceBuff;
class ABuff;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DUSKBORN_API UBuffManagerComponent : public UActorComponent
{
	GENERATED_BODY()
	//-----------------------Variables----------------------------------------------------------------------------------  

	//public variables
public:
	//protected variables
protected:
	//Classes and references for Buffs

	UPROPERTY(EditDefaultsOnly, Category="Buffs")
	TSubclassOf<ACritChanceBuff> CritChanceBuffClass = nullptr;
	UPROPERTY()
	ACritChanceBuff* CritChanceBuff = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Buffs")
	TSubclassOf<AAttackDamageBuff> AttackDamageBuffClass = nullptr;
	UPROPERTY()
	AAttackDamageBuff* AttackDamageBuff = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Buffs")
	TSubclassOf<AHealthRegBuff> HealthRegBuffClass = nullptr;
	UPROPERTY()
	AHealthRegBuff* HealthRegBuff = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Buffs")
	TSubclassOf<AJumpPowerBuff> JumpPowerBuffClass = nullptr;
	UPROPERTY()
	AJumpPowerBuff* JumpPowerBuff = nullptr;

	/** Class for buff which adds critical chance */
	UPROPERTY(EditDefaultsOnly, Category="Buffs")
	TSubclassOf<AMovementSpeedBuff> MovementSpeedBuffClass = nullptr;
	/** Crit Chance Buff reference */
	UPROPERTY()
	AMovementSpeedBuff* MovementSpeedBuff = nullptr;

	/** Class for buff which adds critical chance */
	UPROPERTY(EditDefaultsOnly, Category="Buffs")
	TSubclassOf<APullSpeedBuff> PullSpeedBuffClass = nullptr;
	/** Crit Chance Buff reference */
	UPROPERTY()
	APullSpeedBuff* PullSpeedBuff = nullptr;

	//private variables
private:
	//-----------------------Functions----------------------------------------------------------------------------------

	//public functions  
public:
	UBuffManagerComponent();

	UFUNCTION(BlueprintCallable)
	ABuffInstance* ActivateBuff(EStats Stat, float BuffAmount);

	/** Will spawn a buff at player location and attach it */
	template <class T>
	T* SpawnBuff(TSubclassOf<T> Class)
	{
		if (!PLAYER)
		{
			Log::Print("Couldn't find player in buff manager: SpawnBuff()");
			return nullptr;
		}
		if (!Class)
		{
			Log::Print("No class for buff selected: SpawnBuff()");
			return nullptr;
		}
		//Spawn Buff in World
		T* SpawnedBuff = GetWorld()->SpawnActor<T>(Class);

		//Attach Buff to player character
		SpawnedBuff->AttachToComponent(PLAYER->BuffLocation, FAttachmentTransformRules::SnapToTargetIncludingScale);

		//return the spawned buff
		return SpawnedBuff;
	}

	/** Will create an instance of the buff */
	template <class T>
	T* SpawnBuffInstance()
	{
		if (!PLAYER)
		{
			Log::Print("Couldn't find player in buff manager: SpawnBuff()");
			return nullptr;
		}
		//Spawn instance of a buff in world
		T* Instance = GetWorld()->SpawnActor<T>(T::StaticClass());

		//Return the spawned instance
		return Instance;
	}

	void CleanUp() const;

	//protected functions
protected:
	virtual void BeginPlay() override;

	//private functions
private:
	ABuffInstance* CreateBuffInstance(ABuff* Buff, const float BuffAmount, const EStats Stat);
	
};
