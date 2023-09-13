// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Prosthesis.generated.h"

class APlayerCharacter;
class UProsthesisBaseState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAbilityUsed);

UCLASS()
class DUSKBORN_API AProsthesis : public AActor
{
	GENERATED_BODY()

	//-----------------------Variables----------------------------------------------------------------------------------  

	//public variables
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* ProthesisImage = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* ProthesisAbilityImage = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* Mesh = nullptr;
	UPROPERTY()
	APlayerCharacter* Player = nullptr;

	UPROPERTY()
	FTimerHandle PrimaryAttackCooldownTimer;
	UPROPERTY()
	FTimerHandle SpecialCooldownTimer;

	UPROPERTY(BlueprintAssignable)
	FAbilityUsed AbilityUsed;

	/** Multiplier for primary attack damage. */
	UPROPERTY(EditAnywhere, Category="Duskborn|Primary")
	float PrimaryDamageScaler = 1.f;
	/** Multiplier for special attack damage. */
	UPROPERTY(EditAnywhere, Category="Duskborn|Special")
	float SpecialDamageScalar = 1.f;
	/** Defines the base attack speed of the prosthesis. Will later be modified by character attack speed. */
	UPROPERTY(EditAnywhere, Category="Duskborn|Primary")
	float ProsthesisAttackSpeed = 1.f;
	/** Cooldown for special ability in seconds */
	UPROPERTY(EditAnywhere, Category="Duskborn|Special")
	float SpecialCooldown = 10.f;

	/** Will add the value to the current CriticalStrikeChance of the player */
	UPROPERTY(EditAnywhere, Category="Duskborn")
	float AdditionalCriticalStrikeChance = 0.f;

	/** Will add the value to the current CriticalStrikeMultiplier of the player */
	UPROPERTY(EditAnywhere, Category="Duskborn")
	float AdditionalCriticalStrikeMultiplier = 0.f;
	UPROPERTY(BlueprintReadOnly)
	bool IsActive = false;

	bool CanUsePrimary = true;
	bool CanUseSpecial = true;

	//protected variables
protected:
	//private variables
private:
	//-----------------------Functions----------------------------------------------------------------------------------

	//public functions  
public:
	AProsthesis();

	virtual bool Init(APlayerCharacter* SetOwner);

	/** Getter for prosthesis combat states */
	virtual UProsthesisBaseState* GetPrimaryAttackState() { return nullptr; }
	virtual UProsthesisBaseState* GetSpecialState() { return nullptr; }

	/** AttachProsthesis to new parent  */
	void AttachProsthesis(USceneComponent* ParentActorComponent, FName SocketName);

	/** Set primary attack on cooldown */
	void SetPrimaryAttackCooldown();
	/** Set special attack on cooldown */
	void SetSpecialCooldown();

	/** reduce the current special cooldown by a defined amount. Reduction is a percent value */
	UFUNCTION(BlueprintCallable)
	void ReduceCurrentSpecialCooldown(float Reduction);

	/** Play montages for all body parts */
	void PlayAnimMontage(UAnimMontage* MontageBody = nullptr, UAnimMontage* MontageLeft = nullptr,
	                     UAnimMontage* MontageRight = nullptr, float PlayRate = 1.f) const;

	UFUNCTION(BlueprintCallable)
	float GetSpecialCooldown();
	UFUNCTION(BlueprintCallable)
	float GetSpecialRemainingCooldown();

	virtual void DestroyProsthesis();

	//protected functions
protected:
	void EnablePrimaryAttack();
	void EnableSpecial();
	virtual void Tick(float DeltaSeconds) override;

	//private functions  
private:
};
