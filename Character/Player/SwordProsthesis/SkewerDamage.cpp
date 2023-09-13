// Fill out your copyright notice in the Description page of Project Settings.


#include "SkewerDamage.h"

#include "Components/CapsuleComponent.h"


// Sets default values
ASkewerDamage::ASkewerDamage() : ADamageObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	DamageMesh = CreateDefaultSubobject<UCapsuleComponent>("Damage Mesh");
	DamageMesh->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void ASkewerDamage::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASkewerDamage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Do damage to enemies
	GenerateHitEvents(DamageMesh);
}
