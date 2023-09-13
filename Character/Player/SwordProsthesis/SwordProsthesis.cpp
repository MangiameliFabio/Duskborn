// Fill out your copyright notice in the Description page of Project Settings.


#include "SwordProsthesis.h"

#include <Duskborn/Character/BaseClasses/DamageObject.h>

#include "SlashDamage.h"
#include "SwordStates.h"
#include "Duskborn/DuskbornGameModeBase.h"
#include "Duskborn/Character/BaseClasses/State.h"
#include "Duskborn/Character/Player/PlayerCharacter.h"
#include "Duskborn/Character/Player/StateMachine/PlayerStateMachine.h"
#include "Duskborn/Management/WorldManager.h"
#include "Duskborn/UpgradeSystem/TriggerManager.h"

ASwordProsthesis::ASwordProsthesis():AProsthesis()
{
}

UProsthesisBaseState* ASwordProsthesis::GetPrimaryAttackState()
{
	Super::GetPrimaryAttackState();
	return SlashState;
}

UProsthesisBaseState* ASwordProsthesis::GetSpecialState()
{
	Super::GetSpecialState();
	
	if (!GetWorld())
	{
		Log::Print("World not found");
		return nullptr;
	}
	TRIGGER_MANAGER->Notify(PLAYER, ETriggerTypes::ON_LEFT_SPECIAL);

	return SkewerState;
}

ADamageObject* ASwordProsthesis::SpawnDamageObject(float DamageScale, TSubclassOf<ADamageObject> DamageClass) const
{
	if (!GetWorld())
	{
		Log::Print("World not found");
		return nullptr;
	}
	ADamageObject* DamageObject = PLAYER->GetWorld()->SpawnActor<ADamageObject>(DamageClass,
		PLAYER->GetActorLocation(),
		PLAYER->GetActorRotation());
	DamageObject->Init(DamageScale, AdditionalCriticalStrikeChance,
	                   AdditionalCriticalStrikeMultiplier,PLAYER);
	DamageObject->AttachToActor(PLAYER, FAttachmentTransformRules::SnapToTargetIncludingScale,
	                            "CharacterOriginSocket");

	return DamageObject;
}

bool ASwordProsthesis::Init(APlayerCharacter* SetOwner)
{
	if (!Super::Init(SetOwner))
		return false;

	SlashState = NewObject<USwordSlash>();
	SlashState->Init(Player->StateMachine, this);
	SkewerState = NewObject<USwordSkewer>();
	SkewerState->Init(Player->StateMachine, this);

	if (SlashState)
		return true;

	Log::Print("FirstSlashState is a nullptr in Rifle Prosthesis");
	return false;
}

void ASwordProsthesis::SetSkewerDuration()
{
	check(GetWorld())
	GetWorldTimerManager().SetTimer(SkewerDurationTimer, this, &ASwordProsthesis::DisableSkewer, SkewerDuration);
}

void ASwordProsthesis::DisableSkewer()
{
	check(GetWorld())
	GetWorldTimerManager().ClearTimer(SkewerDurationTimer);
	SkewerShouldFinish = true;
}

void ASwordProsthesis::HandleCombo()
{
	ComboShouldTick = true;

	if (ComboCount < 2)
	{
		ComboCount++;
	}
	else
	{
		ResetComboCount();
	}
}

void ASwordProsthesis::ResetComboCount()
{
	ComboCount = 0;
}

void ASwordProsthesis::BeginPlay()
{
	Super::BeginPlay();
}

void ASwordProsthesis::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//If ComboShouldTick is true start counting down the time. After the specified amount of time the combo counter is resetet
	if (ComboShouldTick)
	{
		if (CurrentBufferDuration >= ComboBufferDuration)
		{
			CurrentBufferDuration = 0;
			ComboShouldTick = false;

			ResetComboCount();
		}
		else
		{
			CurrentBufferDuration += DeltaSeconds;
		}
	}
}
