// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructorDamage.h"

#include "Components/StaticMeshComponent.h"


// Sets default values
ADestructorDamage::ADestructorDamage()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	DamageOrigin = CreateDefaultSubobject<USceneComponent>("PivotPoint");
	DamageOrigin->SetupAttachment(RootComponent);

	DamageMesh = CreateDefaultSubobject<UStaticMeshComponent>("DamageMesh");
	DamageMesh->SetupAttachment(DamageOrigin);
}

void ADestructorDamage::ChangeCollisionSize(const float Distance) const
{
	//DamageOrigin is the parent component of damage mesh and therefore will also change the scale of the mesh.

	check(DamageOrigin);
	const float Scale = Distance / 100.f; //100 is the height of the static mesh cylinder
	DamageOrigin->SetRelativeScale3D(FVector(1, 1, Scale));
}
