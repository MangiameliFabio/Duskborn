// Fill out your copyright notice in the Description page of Project Settings.


#include "Buff.h"
#include "BuffInstance.h"
#include "NiagaraComponent.h"
#include "Duskborn/Helper/Log.h"


// Sets default values
ABuff::ABuff() : AActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	BuffNiagara = CreateDefaultSubobject<UNiagaraComponent>("Buff Effect");
	BuffNiagara->SetupAttachment(Root);
}

void ABuff::AddInstance(ABuffInstance* Instance)
{
	InstanceCount++;
	BuffInstances.Add(Instance);

	//If there is more then one instance of the buff currently active activate the niagara system
	check(BuffNiagara);
	if (InstanceCount > 0 && !BuffNiagara->IsActive())
		BuffNiagara->Activate();
}

void ABuff::InstanceRemoved(ABuffInstance* Instance)
{
	InstanceCount--;
	BuffInstances.Remove(Instance);

	//If there is no instance of the buff currently active deactivate the niagara system
	check(BuffNiagara);
	if (InstanceCount <= 0)
		BuffNiagara->Deactivate();
}

void ABuff::CleanUp()
{
	if (!BuffInstances.IsEmpty())
	{
		const int Iterations = BuffInstances.Num();
		for (int i = 0; i < Iterations; ++i)
		{
			if (BuffInstances[i])
				BuffInstances[i]->CleanUp();
		}
	}
	Destroy();
}
