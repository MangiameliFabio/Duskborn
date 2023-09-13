// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageObject.h"
#include "Duskborn/Character/BaseClasses/BaseCharacter.h"


// Sets default values
ADamageObject::ADamageObject() : AActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

bool ADamageObject::GenerateHitEvents(const UPrimitiveComponent* Collision)
{
	bool CharactersHit = false;

	check(Collision)
	//Get overlapping character
	Collision->GetOverlappingActors(OverlappingCharacter, CharacterClass);

	if (!OverlappingCharacter.IsEmpty())
	{
		for (int i = 0; i < OverlappingCharacter.Num(); ++i)
		{
			//Check if overlapping actor is valid
			if (ABaseCharacter* Character = Cast<ABaseCharacter>(OverlappingCharacter[i]))
			{
				//Check if character is not dead
				if (!Character->IsDead)
				{
					// If  Character can't be found in HitCharacterArray continue
					if (HitCharacters.Find(Character) == INDEX_NONE)
					{
						//Damage logic
						AttackingCharacter->DoDamageToCharacter(Character, DamageScaler, AdditionalCriticalStrikeChance,
						                                        AdditionalCriticalStrikeMultiplier);

						//Add character to array
						HitCharacters.Add(Character);
						CharactersHit = true;
					}
				}
			}
		}
	}

	return CharactersHit;
}

void ADamageObject::ClearHitCharacters()
{
	HitCharacters.Empty();
}

void ADamageObject::Init(const float NewDamageScalar, const float NewAdditionalCriticalStrikeChance,
                         const float NewAdditionalCriticalStrikeMultiplier, ABaseCharacter* NewAttackingCharacter)
{
	DamageScaler = NewDamageScalar;
	AdditionalCriticalStrikeChance = NewAdditionalCriticalStrikeChance;
	AdditionalCriticalStrikeMultiplier = NewAdditionalCriticalStrikeMultiplier;
	AttackingCharacter = NewAttackingCharacter;
}

// Called every frame
void ADamageObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
