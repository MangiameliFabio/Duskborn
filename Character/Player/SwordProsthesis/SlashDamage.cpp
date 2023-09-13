// Fill out your copyright notice in the Description page of Project Settings.


#include "SlashDamage.h"

#include "Duskborn/DuskbornGameModeBase.h"
#include "Duskborn/Management/WorldManager.h"
#include "Duskborn/UpgradeSystem/TriggerManager.h"


// Sets default values
ASlashDamage::ASlashDamage() : ADamageObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	DamageMesh = CreateDefaultSubobject<UStaticMeshComponent>("Damage Mesh");
	DamageMesh->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void ASlashDamage::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASlashDamage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	check(GetWorld())

	//Generate hit events as long as the damage object is alive
	if (GenerateHitEvents(DamageMesh))
	{
		if (!HitCharacters.IsEmpty())
		{
			for (const auto HitCharacter : HitCharacters)
			{
				TRIGGER_MANAGER->Notify(HitCharacter, ETriggerTypes::ON_LEFT_PRIMARY);
			}
		}
	}
}

void ASlashDamage::SetComboCount(uint8 NewComboCount)
{
	ComboCount = NewComboCount;
	InitBlueprint();
}
