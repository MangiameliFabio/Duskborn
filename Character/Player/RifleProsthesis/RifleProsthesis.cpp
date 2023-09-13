// Fill out your copyright notice in the Description page of Project Settings.


#include "RifleProsthesis.h"

#include "RifleProjectile.h"
#include "RifleStates.h"
#include "Duskborn/DuskbornGameModeBase.h"
#include "Duskborn/Character/Enemy/EnemyCharacter.h"
#include "Duskborn/Character/Player/PlayerCharacter.h"
#include "Duskborn/Character/Player/StateMachine/PlayerStateMachine.h"
#include "Duskborn/Management/WorldManager.h"
#include "Duskborn/UpgradeSystem/TriggerManager.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ARifleProsthesis::ARifleProsthesis() : AProsthesis()
{
}

UProsthesisBaseState* ARifleProsthesis::GetPrimaryAttackState()
{
	Super::GetPrimaryAttackState();
	check(FireState);
	return FireState;
}

UProsthesisBaseState* ARifleProsthesis::GetSpecialState()
{
	Super::GetSpecialState();
	check(KillerInstinctState);
	return KillerInstinctState;
}

bool ARifleProsthesis::Init(APlayerCharacter* SetOwner)
{
	if (!Super::Init(SetOwner))
		return false;

	FireState = NewObject<URifleFireState>();
	FireState->Init(Player->StateMachine, this);
	KillerInstinctState = NewObject<UKillerInstinctState>();
	KillerInstinctState->Init(Player->StateMachine, this);

	if (FireState && KillerInstinctState)
		return true;

	Log::Print("FireState or KillerInstinctState is a nullptr in Rifle Prosthesis");
	return false;
}

void ARifleProsthesis::SpawnProjectile(FVector Target)
{
	if (!RifleProjectileClass)
	{
		Log::Print("No projectile class in rifle prosthesis");
		return;
	}
	if (!GetWorld())
	{
		Log::Print("World not found");
		return;
	}
	const FVector Location = Mesh->GetSocketLocation("MuzzleSocket");

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	//Spawn Projectile
	if (const ARifleProjectile* Projectile = GetWorld()->SpawnActor<ARifleProjectile>(RifleProjectileClass,
		Location,
		Player->GetActorRotation(),
		SpawnParams))
	{
		//Calculate the projectile destination
		FVector AimVector = Target - Location;

		//Get aim direction
		AimVector.Normalize();

		//Move the projectile
		Projectile->ProjectileMovementComponent->Velocity = AimVector * Projectile->ProjectileMovementComponent->
			InitialSpeed;
	}
}

void ARifleProsthesis::CheckForHit(const FHitResult& Hit)
{
	if (Hit.bBlockingHit)
	{
		//Spawn particle if hit is true
		SpawnHitParticle(Hit.Location, Hit.Normal.Rotation());

		if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Hit.GetActor()))
		{
			if (!Enemy->IsDead)
			{
				//If hit actor is an enemy and not dead notify all items with OnRightPrimary trigger
				TRIGGER_MANAGER->Notify(Enemy, ETriggerTypes::ON_RIGHT_PRIMARY, &Hit.Location);

				//do damage to hit enemy and use rifle damage modifier
				Player->DoDamageToCharacter(Enemy, PrimaryDamageScaler, AdditionalCriticalStrikeChance,
				                            AdditionalCriticalStrikeMultiplier);
			}
		}
	}
}

void ARifleProsthesis::ActivateKillerInstinct()
{
	if (!GetWorld())
	{
		Log::Print("World not found");
		return;
	}
	//Set timer to deactivate killer instinct in callback
	GetWorldTimerManager().SetTimer(KillerInstinctTimer, this, &ARifleProsthesis::DeactivateKillerInstinct,
	                                KillerInstinctDuration);

	// Get the game world's time dilation
	const float TimeDilation = GetWorld()->GetWorldSettings()->GetEffectiveTimeDilation();

	// Set the time dilation factor to a value less than 1 to slow down the game world
	GetWorld()->GetWorldSettings()->SetTimeDilation(TimeDilation * KillerInstinctSlowScale);

	// Activate Niagara via blueprint
	ActivateKillerInstinctParticles();

	//Trigger all Items with OnRightSpecial trigger
	TRIGGER_MANAGER->Notify(PLAYER, ETriggerTypes::ON_RIGHT_SPECIAL);

	//Modify sound
	UGameplayStatics::PushSoundMixModifier(this, this->KillerInstinctSoundMix);
	UGameplayStatics::PlaySound2D(this, this->KillerInstinctSound);
}

void ARifleProsthesis::DeactivateKillerInstinct()
{
	GetWorldTimerManager().ClearTimer(KillerInstinctTimer);

	// Restore the original time dilation factor to resume normal gameplay speed
	GetWorld()->GetWorldSettings()->SetTimeDilation(1.0f);

	//Set special on cooldown
	SetSpecialCooldown();
	DeactivateKillerInstinctParticles();

	UGameplayStatics::PopSoundMixModifier(this, this->KillerInstinctSoundMix);
}

void ARifleProsthesis::DestroyProsthesis()
{
	Super::DestroyProsthesis();

	if (GetWorld())
	{
		GetWorldTimerManager().ClearTimer(KillerInstinctTimer);
		GetWorldTimerManager().ClearTimer(IsShootingTimer);
	}
}

void ARifleProsthesis::BeginPlay()
{
	Super::BeginPlay();

	KillerInstinctDuration *= KillerInstinctSlowScale;

	InitBlueprint();
}
