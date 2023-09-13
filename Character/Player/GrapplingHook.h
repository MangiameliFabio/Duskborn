// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrapplingHook.generated.h"

class USphereComponent;
class UCapsuleComponent;
class UPhysicsConstraintComponent;

UCLASS()
class DUSKBORN_API AGrapplingHook : public AActor
{
	GENERATED_BODY()

	//-----------------------Variables----------------------------------------------------------------------------------  

	//public variables
public:
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Hook = nullptr;
	UPROPERTY(EditDefaultsOnly)
	USphereComponent* PlayerAttachmentSphere = nullptr;
	UPROPERTY(EditDefaultsOnly)
	UPhysicsConstraintComponent* PhysicsConstraint = nullptr;

	UPROPERTY(BlueprintReadOnly)
	float CurrentGravity;

	//protected variables
protected:
	//private variables
private:
	UPROPERTY()
	UPrimitiveComponent* GrappledComponent = nullptr;

	//-----------------------Functions----------------------------------------------------------------------------------

	//public functions  
public:
	AGrapplingHook();
	
	void SetupGrapplingHook(const FVector& GrapplingLocation, const FVector& PlayerLocation,
	                        const FVector&, UPrimitiveComponent* NewGrappledComponent = nullptr);

	/** Detach grappling hook from enemy component*/
	void DetachGrapplingHook() const;

	/** Add force to AttachmentSphere to give player controls while swinging*/
	void AddSwingForce(const FVector& SwingForce) const;

	/** Check the amount of down force of AttachmentSphere */
	bool CheckDownForce();

	//protected functions
protected:
	virtual void BeginPlay() override;

	/** Get length of gravity vector */
	FVector GetGravityLength();

	//private functions  
private:
};
