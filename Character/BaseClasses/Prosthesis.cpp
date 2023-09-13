// Fill out your copyright notice in the Description page of Project Settings.


#include "Prosthesis.h"

#include "MovieSceneTracksComponentTypes.h"
#include "State.h"
#include "Duskborn/Management/WorldManager.h"


// Sets default values
AProsthesis::AProsthesis() : AActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	SetRootComponent(Mesh);
}

bool AProsthesis::Init(APlayerCharacter* SetOwner)
{
	Player = SetOwner;

	if (Player)
	{
		return true;
	}
	return false;
}

void AProsthesis::AttachProsthesis(USceneComponent* ParentActorComponent, FName SocketName)
{
	Mesh->AttachToComponent(ParentActorComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
}

void AProsthesis::SetPrimaryAttackCooldown()
{
	CanUsePrimary = false;

	if (!GetWorld())
	{
		Log::Print("World not found in AProsthesis SetPrimaryAttackCooldown()");
		return;
	}
	GetWorldTimerManager().SetTimer(PrimaryAttackCooldownTimer, this, &AProsthesis::EnablePrimaryAttack,
	                                Player->GetAttackSpeedAsMultiplier() * ProsthesisAttackSpeed);
}

void AProsthesis::SetSpecialCooldown()
{
	CanUseSpecial = false;

	if (!GetWorld())
	{
		Log::Print("World not found in AProsthesis SetSpecialCooldown()");
		return;
	}
	GetWorldTimerManager().SetTimer(SpecialCooldownTimer, this, &AProsthesis::EnableSpecial,
	                                SpecialCooldown * (1 - Player->GetCoolDownReduction() / 100.f));

	AbilityUsed.Broadcast();
}

void AProsthesis::ReduceCurrentSpecialCooldown(float Reduction)
{
	if (!GetWorld())
	{
		Log::Print("World not found in AProsthesis ReduceCurrentSpecialCooldown()");
		return;
	}

	//Get remaining cooldown of special ability
	const float RemainingTime = GetWorldTimerManager().GetTimerRemaining(SpecialCooldownTimer);

	//If remaining time is greater than 10% of the general cooldown continue
	if (RemainingTime > SpecialCooldown * 0.1)
	{
		//Subtract the reduction from remaining time
		const float NewRemainingTime = RemainingTime * (1 - Reduction / 100);

		//New remaining time should always be above 0
		if (NewRemainingTime > 0.f)
		{
			//Set timer
			GetWorldTimerManager().SetTimer(SpecialCooldownTimer, this, &AProsthesis::EnableSpecial,
			                                NewRemainingTime, false);
		}
		else
		{
			EnableSpecial();
		}
	}
}

void AProsthesis::PlayAnimMontage(UAnimMontage* MontageBody, UAnimMontage* MontageLeft,
                                  UAnimMontage* MontageRight, float PlayRate) const
{
	/** Get anim instance from character skeletal meshes */
	UAnimInstance* AnimInstanceBody = PLAYER->GetMesh()->GetAnimInstance();
	UAnimInstance* AnimInstanceLeft = PLAYER->LeftProsthesis->Mesh->GetAnimInstance();
	UAnimInstance* AnimInstanceRight = PLAYER->RightProsthesis->Mesh->GetAnimInstance();

	/** Play montage */
	if (MontageBody)
		AnimInstanceBody->Montage_PlayWithBlendSettings(MontageBody, 0.f, PlayRate);
	if (MontageLeft)
		AnimInstanceLeft->Montage_PlayWithBlendSettings(MontageLeft, 0.f, PlayRate);
	if (MontageRight)
		AnimInstanceRight->Montage_PlayWithBlendSettings(MontageRight, 0.f, PlayRate);
}

float AProsthesis::GetSpecialCooldown()
{
	return SpecialCooldown;
}

float AProsthesis::GetSpecialRemainingCooldown()
{
	if (!GetWorld())
	{
		Log::Print("World not found in AProsthesis GetSpecialRemainingCooldown()");
		return -1.f;
	}
	return GetWorldTimerManager().GetTimerRemaining(SpecialCooldownTimer);
}

void AProsthesis::DestroyProsthesis()
{
	if (GetWorld())
	{
		GetWorldTimerManager().ClearTimer(SpecialCooldownTimer);
		GetWorldTimerManager().ClearTimer(PrimaryAttackCooldownTimer);
	}

	GetPrimaryAttackState()->ConditionalBeginDestroy();
	GetSpecialState()->ConditionalBeginDestroy();
}

void AProsthesis::EnablePrimaryAttack()
{
	if (!GetWorld())
	{
		Log::Print("World not found in AProsthesis EnablePrimaryAttack()");
		return;
	}
	GetWorldTimerManager().ClearTimer(PrimaryAttackCooldownTimer);
	CanUsePrimary = true;
}

void AProsthesis::EnableSpecial()
{
	if (!GetWorld())
	{
		Log::Print("World not found AProsthesis EnableSpecial()");
		return;
	}
	GetWorldTimerManager().ClearTimer(SpecialCooldownTimer);
	CanUseSpecial = true;
}

void AProsthesis::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
