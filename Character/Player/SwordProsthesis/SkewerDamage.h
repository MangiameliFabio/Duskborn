// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Duskborn/Character/BaseClasses/DamageObject.h"
#include "SkewerDamage.generated.h"

UCLASS()
class DUSKBORN_API ASkewerDamage : public ADamageObject
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASkewerDamage();

	UPROPERTY(EditAnywhere)
	USceneComponent* Root = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCapsuleComponent* DamageMesh = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
