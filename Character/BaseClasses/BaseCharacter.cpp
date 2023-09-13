// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "Duskborn/Helper/Log.h"
#include "Duskborn/UI/CombatNumberComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ABaseCharacter::ABaseCharacter() : ACharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CombatNumbers = CreateDefaultSubobject<UCombatNumberComponent>("Combat Number");

	CombatNumberLocation = CreateDefaultSubobject<USceneComponent>("CombatNumberLocation");
	CombatNumberLocation->SetupAttachment(RootComponent);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Set current health to max health
	SetCurrentHealth(GetMaxHealth());
	//Set base health to max health
	BaseMaxHealth = GetMaxHealth();

	if (!GetCharacterMovement())
	{
		Log::Print("No Movement Component found on Player Character");
		return;
	}

	//Set max walk speed of character movement
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;

	//If the character doesn't need health regeneration don't start timer. If later in the game the timer needs to be started, use SetHealthReg();
	if (HealthRegeneration > 0.f)
		SetHealthReg();

	BaseCoolDownReduction = CoolDownReduction;
	BaseAttackSpeed = AttackSpeed;
}

float ABaseCharacter::GetMaxHealth() const
{
	return MaxHealth;
}

void ABaseCharacter::SetMaxHealth(float NewMaxHealth)
{
	this->MaxHealth = NewMaxHealth;
}

float ABaseCharacter::GetCurrentHealth() const
{
	return CurrentHealth;
}

void ABaseCharacter::SetCurrentHealth(float NewCurrentHealth)
{
	this->CurrentHealth = NewCurrentHealth;
}

float ABaseCharacter::GetAttackDamage() const
{
	return AttackDamage;
}

void ABaseCharacter::SetAttackDamage(float NewAttackDamage)
{
	this->AttackDamage = NewAttackDamage;
}

float ABaseCharacter::GetAttackSpeed() const
{
	return AttackSpeed;
}

void ABaseCharacter::SetAttackSpeed(float NewAttackSpeed)
{
	//Clamp attack speed to 10%
	if (NewAttackSpeed <= 10.f)
		this->AttackSpeed = 10.f;
	else
		this->AttackSpeed = NewAttackSpeed;
}

float ABaseCharacter::GetCriticalStrikeChance() const
{
	return CriticalStrikeChance;
}

void ABaseCharacter::SetCriticalStrikeChance(float NewCriticalStrikeChance)
{
	this->CriticalStrikeChance = NewCriticalStrikeChance;
}

float ABaseCharacter::GetCriticalStrikeMultiplier() const
{
	return CriticalStrikeMultiplier;
}

void ABaseCharacter::SetCriticalStrikeMultiplier(float NewCriticalStrikeMultiplier)
{
	this->CriticalStrikeMultiplier = NewCriticalStrikeMultiplier;
}

float ABaseCharacter::GetMovementSpeed() const
{
	return MovementSpeed;
}

void ABaseCharacter::SetMovementSpeed(float NewMovementSpeed)
{
	this->MovementSpeed = NewMovementSpeed;
	GetCharacterMovement()->MaxWalkSpeed = NewMovementSpeed;
}

bool ABaseCharacter::GetIsDead() const
{
	return IsDead;
}

void ABaseCharacter::SetIsDead(bool bIsDead)
{
	IsDead = bIsDead;
}

float ABaseCharacter::GetCoolDownReduction() const
{
	return CoolDownReduction;
}

void ABaseCharacter::SetCoolDownReduction(float NewSpecialCoolDownReduction)
{
	if (CoolDownReduction > 90.f)
		this->CoolDownReduction = 90.f;
	else if (CoolDownReduction < 0.f)
		this->CoolDownReduction = 0.f;
	else
		this->CoolDownReduction = NewSpecialCoolDownReduction;
}

float ABaseCharacter::GetBaseAttackSpeed() const
{
	return BaseAttackSpeed;
}

void ABaseCharacter::SetBaseAttackSpeed(float NewBaseAttackSpeed)
{
	this->BaseAttackSpeed = NewBaseAttackSpeed;
}

float ABaseCharacter::GetBaseCoolDownReduction() const
{
	return BaseCoolDownReduction;
}

void ABaseCharacter::SetBaseCoolDownReduction(float NewBaseCoolDownReduction)
{
	this->BaseCoolDownReduction = NewBaseCoolDownReduction;
}

float ABaseCharacter::GetAttackSpeedAsMultiplier() const
{
	return 1 / (GetAttackSpeed() / 100);
}

bool ABaseCharacter::Heal(float HealAmount)
{
	//Only heal when health is below max
	if (GetCurrentHealth() < GetMaxHealth())
	{
		SetCurrentHealth(GetCurrentHealth() + HealAmount);

		//Set CurrentHealth to MaxHealth if above
		if (GetCurrentHealth() > GetMaxHealth())
		{
			SetCurrentHealth(GetMaxHealth());
		}

		//Use combat numbers for healing representation
		HealAmount = FMath::RoundToFloat(HealAmount);
		CombatNumbers->AddCombatNumber(FText::AsNumber(HealAmount, &FNumberFormattingOptions::DefaultNoGrouping()),
		                               CombatNumberLocation, FLinearColor::Green);

		return true;
	}
	return false;
}

bool ABaseCharacter::CheckForCrit(float AdditionalCriticalChance)
{
	return FMath::FRandRange(0.f, 100.f) <= CriticalStrikeChance + AdditionalCriticalChance;
}

FVector ABaseCharacter::GetHorizontalVelocity() const
{
	return FVector(GetCharacterMovement()->Velocity.X, GetCharacterMovement()->Velocity.Y, 0);
}


void ABaseCharacter::CleanUp()
{
	if (!GetWorld())
	{
		Log::Print("No world object found in base character CleanUp()");
		return;
	}
	GetWorldTimerManager().ClearAllTimersForObject(this);
}

void ABaseCharacter::ApplyHealthRegeneration()
{
	Heal(HealthRegeneration);
}

void ABaseCharacter::SetHealthReg()
{
	GetWorldTimerManager().SetTimer(HealthRegTimer, this, &ABaseCharacter::ApplyHealthRegeneration,
	                                HealthRegenerationRate, true);
}

float ABaseCharacter::GetHealthRegeneration() const
{
	return HealthRegeneration;
}

void ABaseCharacter::SetHealthRegeneration(float NewHealthRegeneration)
{
	this->HealthRegeneration = NewHealthRegeneration;
}

void ABaseCharacter::ReceiveDamage(float ReceivedDamage, bool Critical)
{
	if (!IsDead)
	{
		SetCurrentHealth(GetCurrentHealth() - ReceivedDamage);
		if (GetCurrentHealth() <= 0)
		{
			Die();
		}
	}
}

float ABaseCharacter::DoDamageToCharacter(ABaseCharacter* OtherCharacter, const float DamageScaler,
                                          const float AdditionalCriticalChance, const float AdditionalCriticalMulti)
{
	//Multiply scale with character attack damage
	float DamageToApply = AttackDamage * DamageScaler;
	bool Critical = false;

	//Check if hit should be critical
	if (CheckForCrit(AdditionalCriticalChance))
	{
		//Add critical multiplier
		DamageToApply *= CriticalStrikeMultiplier + AdditionalCriticalMulti;

		//Trigger Event
		OnCriticalDamage(DamageToApply, OtherCharacter);
		Critical = true;
	}
	else
	{
		//Trigger Event
		OnNormalDamage(DamageToApply, OtherCharacter);
	}

	//Call ReceiveDamage() function on other character to apply damage
	OtherCharacter->ReceiveDamage(DamageToApply, Critical);

	return DamageToApply;
}

void ABaseCharacter::Die()
{
	OnDie();
	CleanUp();
	SetIsDead(true);
	SetCurrentHealth(0.f);
	GetCapsuleComponent()->SetCollisionProfileName("Corpse");
	GetMovementComponent()->Velocity = FVector::Zero();
}
