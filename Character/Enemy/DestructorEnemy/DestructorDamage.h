// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Duskborn/Character/BaseClasses/DamageObject.h"
#include "Components/StaticMeshComponent.h"

#include "DestructorDamage.generated.h"

UCLASS()
class DUSKBORN_API ADestructorDamage : public ADamageObject
{
	GENERATED_BODY()

	//-----------------------Variables----------------------------------------------------------------------------------  

	//public variables
public:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* DamageMesh = nullptr;
	
	//protected variables
protected:
	UPROPERTY(EditAnywhere)
	USceneComponent* Root = nullptr;
	UPROPERTY(EditAnywhere)
	USceneComponent* DamageOrigin = nullptr;

	//private variables
private:
	//-----------------------Functions----------------------------------------------------------------------------------

	//public functions  
public:
	ADestructorDamage();

	/* Change size of DamageMesh**/
	void ChangeCollisionSize(float Distance) const;
	//protected functions
protected:
	//private functions  
private:
};
