// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Duskborn/Character/BaseClasses/Prosthesis.h"
#include "GameFramework/Actor.h"
#include "RifleProsthesis.generated.h"

class UNiagaraSystem;
class UKillerInstinctState;
class UPlayerBaseState;
class URifleFireState;

UCLASS()
class DUSKBORN_API ARifleProsthesis : public AProsthesis
{
	GENERATED_BODY()

	//-----------------------Variables----------------------------------------------------------------------------------  

	//public variables
public:
	FTimerHandle KillerInstinctTimer;
	FTimerHandle IsShootingTimer;
	
	/** Projectile class to spawn. */
	UPROPERTY(EditAnywhere, Category="Duskborn|Primary")
	TSubclassOf<class ARifleProjectile> RifleProjectileClass = nullptr;
	UPROPERTY()
	URifleFireState* FireState = nullptr;
	UPROPERTY()
	UKillerInstinctState* KillerInstinctState = nullptr;
	
	UPROPERTY(EditAnywhere, Category="Duskborn|Animation")
	UAnimMontage* KillerInstinctBody = nullptr;
	UPROPERTY(EditAnywhere, Category="Duskborn|Animation")
	UAnimMontage* KillerInstinctRight = nullptr;
	UPROPERTY(EditAnywhere, Category="Duskborn|Animation")
	UAnimMontage* ShootRight = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Duskborn|Sound")
	USoundBase* ProjectileHitSound = nullptr;
	UPROPERTY(EditAnywhere, Category = "Duskborn|Sound")
	USoundBase* ShootingSound = nullptr;
	UPROPERTY(EditAnywhere, Category = "Duskborn|Sound")
	USoundBase* KillerInstinctSound = nullptr;
	UPROPERTY(EditAnywhere, Category = "Duskborn|Sound")
	USoundMix* KillerInstinctSoundMix = nullptr;

	/** Defines how much the killer instinct will slow down the time. 1 is normal speed */
	UPROPERTY(EditAnywhere, Category="Duskborn|Special")
	float KillerInstinctSlowScale = 0.5f;

	/** Duration of the KillerInstinct time slow down in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Duskborn|Special")
	float KillerInstinctDuration = 10.f;

	/** Rifle primary attack range */
	UPROPERTY(EditAnywhere, Category = "Duskborn|Primary")
	float RifleRange = 25000.f;
	
	UPROPERTY(BlueprintReadOnly)
	bool IsShooting = false;

	//protected variables
protected:
	//private variables
private:
	//-----------------------Functions----------------------------------------------------------------------------------

	//public functions  
public:
	ARifleProsthesis();

	virtual UProsthesisBaseState* GetPrimaryAttackState() override;
	virtual UProsthesisBaseState* GetSpecialState() override;

	virtual bool Init(APlayerCharacter* SetOwner) override;

	/** Spawn a projectile which flies towards target*/
	UFUNCTION(BlueprintCallable)
	void SpawnProjectile(FVector Target);

	/** Check if a hit occured and than spawn the HitParticles. If the hit actor is an enemy, apply damage */
	void CheckForHit(const FHitResult& Hit);

	/** Slowdown time and change sounds will also start the timer for deactivation */
	void ActivateKillerInstinct();
	/** Change time and sound to normal and set killer instinct on cooldown */
	void DeactivateKillerInstinct();

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnHitParticle(FVector Location, FRotator Rotation);
	UFUNCTION(BlueprintImplementableEvent)
	void ActivateKillerInstinctParticles();
	UFUNCTION(BlueprintImplementableEvent)
	void DeactivateKillerInstinctParticles();

	virtual void DestroyProsthesis() override;
	
	//protected functions
protected:
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintImplementableEvent)
	void InitBlueprint();
	//private functions  
private:
};
