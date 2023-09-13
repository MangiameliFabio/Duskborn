// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Duskborn/Character/BaseClasses/DamageObject.h"
#include "SlashDamage.generated.h"

UCLASS()
class DUSKBORN_API ASlashDamage : public ADamageObject
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASlashDamage();

	UPROPERTY(EditAnywhere)
	USceneComponent* Root = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* DamageMesh = nullptr;

	UPROPERTY(BlueprintReadOnly)
	uint8 ComboCount = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Change combo count for different VFX and collision rotations */
	void SetComboCount(uint8 NewComboCount);
	UFUNCTION(BlueprintImplementableEvent)
	void InitBlueprint();
};
