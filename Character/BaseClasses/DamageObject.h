// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "GameFramework/Actor.h"
#include "DamageObject.generated.h"

UCLASS()
class DUSKBORN_API ADamageObject : public AActor
{
	GENERATED_BODY()

	//-----------------------Variables----------------------------------------------------------------------------------  

	//public variables
public:
	//protected variables
protected:
	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> HitCharacters;
	
	//private variables
private:
	UPROPERTY()
	TArray<AActor*> OverlappingCharacter;
	UPROPERTY()
	ABaseCharacter* AttackingCharacter = nullptr;

	TSubclassOf<ABaseCharacter> CharacterClass = ABaseCharacter::StaticClass();

	float DamageScaler = 0.f;
	float AdditionalCriticalStrikeChance = 0.f;
	float AdditionalCriticalStrikeMultiplier = 0.f;
	//-----------------------Functions----------------------------------------------------------------------------------

	//public functions  
public:
	ADamageObject();

	/** Will generate hit events. Hit character will be added to HitCharacters array @return true if hit occured */
	UFUNCTION(BlueprintCallable)
	bool GenerateHitEvents(const UPrimitiveComponent* Collision);

	/** Clear the HitCharacters array to generate hit events again */
	UFUNCTION(BlueprintCallable)
	void ClearHitCharacters();

	UFUNCTION(BlueprintCallable)
	virtual void Init(float NewDamageScalar, float NewAdditionalCriticalStrikeChance,
	                  float NewAdditionalCriticalStrikeMultiplier, ABaseCharacter* NewAttackingCharacter);

	//protected functions
protected:
	virtual void Tick(float DeltaSeconds) override;
	//private functions  
private:
};
