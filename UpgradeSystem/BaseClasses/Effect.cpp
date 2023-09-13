// Fill out your copyright notice in the Description page of Project Settings.


#include "Effect.h"

#include "Duskborn/UpgradeSystem/ItemFactory.h"
#include "Duskborn/Management/WorldManager.h"
#include "Evaluation/IMovieSceneEvaluationHook.h"

AActor* AEffect::GetEffectInstigator()
{
	return EffectInstigator;
}

void AEffect::SetEffectInstigator(AActor* NewEffectTarget)
{
	this->EffectInstigator = NewEffectTarget;
}

FVector AEffect::GetEffectStartLocation() const
{
	return EffectStartLocation;
}

void AEffect::SetEffectStartLocation(const FVector& NewEffectStartLocation)
{
	this->EffectStartLocation = NewEffectStartLocation;
}

// Sets default values
AEffect::AEffect()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}


void AEffect::Init(float NewTriggerScale)
{
	TriggerScale = NewTriggerScale;

}

void AEffect::SetItemUsed(UItem* NewItemUsed)
{
	ItemUsed = NewItemUsed;
}
