// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Duskborn/Character/BaseClasses/Prosthesis.h"
#include "Duskborn/Character/Enemy/EnemyCharacter.h"
#include "SwordProsthesis.generated.h"

class ADamageObject;
class USwordSkewer;
class UNiagaraComponent;
class USwordSlash;

UCLASS()
class DUSKBORN_API ASwordProsthesis : public AProsthesis
{
	GENERATED_BODY()

	//-----------------------Variables----------------------------------------------------------------------------------  

	//public variables
public:
	FTimerHandle SkewerDurationTimer;
	FTimerHandle ComboTimer;
	
	UPROPERTY()
	USwordSlash* SlashState = nullptr;
	UPROPERTY()
	USwordSkewer* SkewerState = nullptr;

	//Animation Montages for Body and Arms	
	UPROPERTY(EditAnywhere, Category="Duskborn|Animation")
	UAnimMontage* FirstSlashMontageBody = nullptr;
	UPROPERTY(EditAnywhere, Category="Duskborn|Animation")
	UAnimMontage* FirstSlashMontageLeft = nullptr;
	UPROPERTY(EditAnywhere, Category="Duskborn|Animation")
	UAnimMontage* FirstSlashMontageRight = nullptr;

	UPROPERTY(EditAnywhere, Category="Duskborn|Animation")
	UAnimMontage* SecondSlashMontageBody = nullptr;
	UPROPERTY(EditAnywhere, Category="Duskborn|Animation")
	UAnimMontage* SecondSlashMontageLeft = nullptr;
	UPROPERTY(EditAnywhere, Category="Duskborn|Animation")
	UAnimMontage* SecondSlashMontageRight = nullptr;

	UPROPERTY(EditAnywhere, Category="Duskborn|Animation")
	UAnimMontage* ThirdSlashMontageBody = nullptr;
	UPROPERTY(EditAnywhere, Category="Duskborn|Animation")
	UAnimMontage* ThirdSlashMontageLeft = nullptr;
	UPROPERTY(EditAnywhere, Category="Duskborn|Animation")
	UAnimMontage* ThirdSlashMontageRight = nullptr;

	UPROPERTY(EditAnywhere, Category="Duskborn|Animation")
	UAnimMontage* SkewerBody = nullptr;
	UPROPERTY(EditAnywhere, Category="Duskborn|Animation")
	UAnimMontage* SkewerRight = nullptr;
	UPROPERTY(EditAnywhere, Category="Duskborn|Animation")
	UAnimMontage* SkewerLeft = nullptr;

	UPROPERTY(EditAnywhere, Category="Duskborn|Primary")
	TSubclassOf<ADamageObject> SlashDamageClass = nullptr;
	UPROPERTY(EditAnywhere, Category="Duskborn|Special")
	TSubclassOf<ADamageObject> SkewerDamageClass = nullptr;
	
	UPROPERTY(EditAnywhere, Category="Duskborn|Primary")
	float ComboBufferDuration = 0.5f;
	UPROPERTY(EditAnywhere, Category="Duskborn|Primary")
	float CurrentBufferDuration = 0.f;

	UPROPERTY(EditAnywhere, Category="Duskborn|Special")
	float SkewerDuration = 0.5f;
	UPROPERTY(EditAnywhere, Category="Duskborn|Special")
	float SkewerStrength = 5000.f;

	uint8 ComboCount = 0;

	bool ComboShouldTick = false;
	bool SkewerShouldFinish = false;

	//protected variables
protected:
	//private variables
private:
	//-----------------------Functions----------------------------------------------------------------------------------

	//public functions  
public:
	ASwordProsthesis();

	virtual bool Init(APlayerCharacter* SetOwner) override;
	virtual UProsthesisBaseState* GetPrimaryAttackState() override;
	virtual UProsthesisBaseState* GetSpecialState() override;

	/** Spawn damage object which can be chosen by class
	 * @return Pointer to the spawned object
	 */
	ADamageObject* SpawnDamageObject(float DamageScale, TSubclassOf<ADamageObject> DamageClass) const;

	void SetSkewerDuration();
	void DisableSkewer();

	/** Count up combo count and reset if value is to high */
	void HandleCombo();

	/** Set combo count to 0*/
	void ResetComboCount();

	//protected functions
protected:
	virtual void BeginPlay() override;

	//private functions  
private:
	virtual void Tick(float DeltaSeconds) override;
};
