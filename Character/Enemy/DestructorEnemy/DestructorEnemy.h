// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Duskborn/Character/Enemy/EnemyCharacter.h"
#include "Duskborn/Character/Player/PlayerCharacter.h"
#include "DestructorEnemy.generated.h"

class UNiagaraComponent;
class ADestructorDamage;

UCLASS()
class DUSKBORN_API ADestructorEnemy : public AEnemyCharacter
{
	GENERATED_BODY()

	//-----------------------Variables----------------------------------------------------------------------------------  

	//public variables
public:
	FTimerHandle ChargingTimer;
	FTimerHandle LaserTimer;
	FTimerHandle ReloadTimer;
	FTimerHandle ReceivedDamageTimer;
	FTimerHandle RangeCheckTimer;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ADestructorDamage> DestructorDamageClass = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Platform = nullptr;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Turntable = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Cannon = nullptr;
	UPROPERTY(EditAnywhere)
	USceneComponent* Muzzle = nullptr;
	UPROPERTY()
	ADestructorDamage* DestructorDamage = nullptr;

	/** Sound for Charging */
	UPROPERTY(EditAnywhere, Category="Duskborn|Sound", BlueprintReadOnly)
	USoundBase* ChargingSound = nullptr;
	UPROPERTY(BlueprintReadOnly, Category="Duskborn|Sound")
	UAudioComponent* ChargingSoundAudioComponent = nullptr;

	/** Sound for Firing */
	UPROPERTY(EditAnywhere, Category="Duskborn|Sound", BlueprintReadOnly)
	USoundBase* FiringSound = nullptr;
	UPROPERTY(BlueprintReadOnly, Category="Duskborn|Sound")
	UAudioComponent* FiringSoundAudioComponent = nullptr;

	/** Distance in which the destructor will start attacking the player character */
	UPROPERTY(EditAnywhere, Category="Duskborn")
	float DetectionRange = 30000.f;

	/** Rotation speed of the turntable. Only horizontal rotation */
	UPROPERTY(EditAnywhere, Category="Duskborn")
	float TurntableSpeed = 5.f;

	/** Rotation speed of the cannon. Only vertical rotation */
	UPROPERTY(EditAnywhere, Category="Duskborn")
	float CannonSpeed = 5.f;

	/** Min rotation angle in degree for the canon. If value is to low cannon will glitch thorough the mesh */
	UPROPERTY(EditAnywhere, Category="Duskborn")
	float CannonMinAngle = -22.5f;

	/** Max rotation angle in degree for the canon. If value is to low cannon will glitch thorough the mesh */
	UPROPERTY(EditAnywhere, Category="Duskborn")
	float CannonMaxAngle = 43.5f;

	/** Duration how long the destructor will charge before firing in seconds.*/
	UPROPERTY(EditAnywhere, Category="Duskborn", BlueprintReadOnly)
	float ChargeDuration = 5.f;

	/** Duration how long the destructor will fire in seconds.*/
	UPROPERTY(EditAnywhere, Category="Duskborn", BlueprintReadOnly)
	float LaserDuration = 10.f;

	/** Duration how long the destructor reload.*/
	UPROPERTY(EditAnywhere, Category="Duskborn")
	float ReloadDuration = 5.f;

	/** Width of the laser beam */
	UPROPERTY(EditAnywhere, Category="Duskborn", BlueprintReadOnly)
	float BeamWidth = 300.f;

	/** True if destructor can fire */
	bool CanFire = false;
	
	/** True if destructor is currently charging */
	bool Charging = false;
	
	/** True if destructor is currently firing the laser */
	bool FiringLaser = false;
	
	/** True if destructor is currently reloading */
	bool Reloaded = false;

	//protected variables
protected:
	//private variables
private:
	//-----------------------Functions----------------------------------------------------------------------------------

	//public functions  
public:
	ADestructorEnemy();

	/** Check if player is in range for destructor attack */
	bool CheckIfPlayerInRange() const;

	/** Rotate turntable and cannon towards the player character */
	void RotateToPlayer(float DeltaTime) const;

	/** Change size of the laser DamageObject and Niagara System */
	void ChangeLaserSize();

	/** Event implemented in blueprints to change Niagara size */
	UFUNCTION(BlueprintImplementableEvent)
	void ChangeNiagaraSize(float Size);

	/** Called when DamageObject should generate hit events */
	void GenerateHits() const;

	/** Will spawn DamageObject at destructor Muzzle location */
	void SpawnDamageObject();
	
	void DeleteDamageObject();

	/** Start sound and niagara for charging. Set ChargingTimer */
	void StartCharging();

	/** Start sound for firing. Spawn DamageObject and set FiringTimer */
	void StartLaser();
	
	/** Deactivate sound and niagara. Delete DamageObject and set reload timer */
	void StartReload();
	
	UFUNCTION(BlueprintImplementableEvent)
	void ActivateNiagara();
	UFUNCTION(BlueprintImplementableEvent)
	void DeactivateNiagara();
	
	virtual void Die() override;

	//protected functions
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	//private functions  
private:
	/** Translate the world rotation to local rotation */
	void WorldToLocalRotation(FRotator& WorldRotation) const;
};
