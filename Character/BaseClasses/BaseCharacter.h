// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UCombatNumberComponent;

UCLASS()
class DUSKBORN_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

	//-----------------------Variables----------------------------------------------------------------------------------  

	//public variables
public:
	FTimerHandle HealthRegTimer;

	UPROPERTY(EditAnywhere)
	UCombatNumberComponent* CombatNumbers = nullptr;
	UPROPERTY(EditAnywhere)
	USceneComponent* CombatNumberLocation = nullptr;

	/** Max amount of character health*/
	UPROPERTY(EditAnywhere, Category="Duskborn|Stats")
	float MaxHealth = 200.f;
	/** Max health at the beginning of the game */
	float BaseMaxHealth = 200.f;
	UPROPERTY()
	/** Amount of health the character currently has */
	float CurrentHealth = MaxHealth;
	/** Amount of health regenerated */
	UPROPERTY(EditAnywhere, Category="Duskborn|Stats")
	float HealthRegeneration = 0.f;
	/** Define the time between regeneration ticks */
	UPROPERTY(EditAnywhere, Category="Duskborn|Stats")
	float HealthRegenerationRate = 1.f;
	/** The current damage value which is used for attacks */
	UPROPERTY(EditAnywhere, Category="Duskborn|Stats")
	float AttackDamage = 10.f;
	/** AttackDamage at the beginning of the game */
	float BaseAttackDamage = AttackDamage;
	/** Attack Speed modifier in percent. 100% should be the normal speed of a hit */
	UPROPERTY(EditAnywhere, Category="Duskborn|Stats")
	float AttackSpeed = 100.f;
	float BaseAttackSpeed = AttackSpeed;
	/** Chance in percent for a critical hit */
	UPROPERTY(EditAnywhere, Category="Duskborn|Stats")
	float CriticalStrikeChance = 0.f;
	/** Amount with which the damage gets multiplied when doing a critical hit */
	UPROPERTY(EditAnywhere, Category="Duskborn|Stats")
	float CriticalStrikeMultiplier = 2.0f;
	/** Character movement speed */
	UPROPERTY(EditAnywhere, Category="Duskborn|Stats")
	float MovementSpeed = 600.0f;
	/** Reduction of cooldown in percent for abilities */
	UPROPERTY(EditAnywhere, Category="Duskborn|Stats")
	float CoolDownReduction = 0.f;
	float BaseCoolDownReduction = CoolDownReduction;

	bool IsDead = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Duskborn|Stats")
	bool Knockbackable = true;

	//protected variables
protected:
	//private variables
private:
	//-----------------------Functions----------------------------------------------------------------------------------

	//public functions  
public:
	ABaseCharacter();

	/** Amount of damage the character should receive */
	UFUNCTION(BlueprintCallable)
	virtual void ReceiveDamage(float ReceivedDamage, bool Critical = false);

	/** Function is called when character is doing damage to other character. Damage can be modified with the scalar. Additional Chance and Multiplier can be used when an attack has special buffs.
	 * @return the applied damage to other character */
	UFUNCTION(BlueprintCallable)
	virtual float DoDamageToCharacter(ABaseCharacter* OtherCharacter, float DamageScaler,
	                                  float AdditionalCriticalChance = 0.f,
	                                  float AdditionalCriticalMulti = 0.f);

	/** Disable character. Character needs to be destroyed manually */
	virtual void Die();
	UFUNCTION(BlueprintImplementableEvent)
	void OnDie();

	/** Level Up the character */
	virtual void LevelUP(float Scalar = 1.f)
	{
	}

	/** Get character velocity without Z axis */
	FVector GetHorizontalVelocity() const;

	/** Trigger event when character did normal damage */
	virtual void OnNormalDamage(float DamageAmount, ABaseCharacter* OtherCharacter)
	{
	};
	/** Trigger event when character did critical damage */
	virtual void OnCriticalDamage(float DamageAmount, ABaseCharacter* OtherCharacter)
	{
	};

	/** Called on Die() or when the game ends/restarts */
	virtual void CleanUp();

	/** Heal character @return true if heal was successful*/
	UFUNCTION(BlueprintCallable)
	virtual bool Heal(float HealAmount);
	/** Check if critical hit should happen @return true if hit is critical*/
	virtual bool CheckForCrit(float AdditionalCriticalChance);

	UFUNCTION(BlueprintCallable)
	virtual float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable)
	virtual void SetMaxHealth(float NewMaxHealth);
	UFUNCTION(BlueprintCallable)
	virtual float GetCurrentHealth() const;
	UFUNCTION(BlueprintCallable)
	virtual void SetCurrentHealth(float NewCurrentHealth);
	UFUNCTION(BlueprintCallable)
	virtual float GetAttackDamage() const;
	UFUNCTION(BlueprintCallable)
	virtual void SetAttackDamage(float NewAttackDamage);
	UFUNCTION(BlueprintCallable)
	virtual float GetAttackSpeed() const;
	UFUNCTION(BlueprintCallable)
	virtual void SetAttackSpeed(float NewAttackSpeed);
	UFUNCTION(BlueprintCallable)
	virtual float GetCriticalStrikeChance() const;
	UFUNCTION(BlueprintCallable)
	virtual void SetCriticalStrikeChance(float NewCriticalStrikeChance);
	UFUNCTION(BlueprintCallable)
	virtual float GetCriticalStrikeMultiplier() const;
	UFUNCTION(BlueprintCallable)
	virtual void SetCriticalStrikeMultiplier(float NewCriticalStrikeMultiplier);
	UFUNCTION(BlueprintCallable)
	virtual float GetMovementSpeed() const;
	UFUNCTION(BlueprintCallable)
	virtual void SetMovementSpeed(float NewMovementSpeed);
	UFUNCTION(BlueprintCallable)
	virtual bool GetIsDead() const;
	UFUNCTION(BlueprintCallable)
	virtual void SetIsDead(bool bIsDead);
	UFUNCTION(BlueprintCallable)
	virtual float GetHealthRegeneration() const;
	UFUNCTION(BlueprintCallable)
	virtual void SetHealthRegeneration(float NewHealthRegeneration);
	UFUNCTION(BlueprintCallable)
	virtual float GetCoolDownReduction() const;
	UFUNCTION(BlueprintCallable)
	virtual void SetCoolDownReduction(float NewSpecialCoolDownReduction);
	UFUNCTION(BlueprintCallable)
	float GetBaseAttackSpeed() const;
	UFUNCTION(BlueprintCallable)
	void SetBaseAttackSpeed(float NewBaseAttackSpeed);
	UFUNCTION(BlueprintCallable)
	float GetBaseCoolDownReduction() const;
	UFUNCTION(BlueprintCallable)
	void SetBaseCoolDownReduction(float NewBaseCoolDownReduction);
	/** @return attack speed of character as a multiplier. E.g. 200% is 0.5 */
	float GetAttackSpeedAsMultiplier() const;

	//protected functions
protected:
	virtual void BeginPlay() override;

	//Use health regeneration with Heal()
	void ApplyHealthRegeneration();
	//Set timer for health regeneration
	void SetHealthReg();
	//private functions  
private:
};
