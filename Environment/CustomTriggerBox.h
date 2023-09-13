// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomTriggerBox.generated.h"

class UBoxComponent;

//Can be used to trigger events in world. For example finishing the level when jumping through the end gate
UCLASS()
class DUSKBORN_API ACustomTriggerBox : public AActor
{
	GENERATED_BODY()
	
	//-----------------------Variables----------------------------------------------------------------------------------  
 
	//public variables
	public:
	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxCollision = nullptr;
 
	//protected variables
	protected:
 
	//private variables
	private:
 
	//-----------------------Functions----------------------------------------------------------------------------------

	//public functions
public:
	// Sets default values for this actor's properties
	ACustomTriggerBox();

	UFUNCTION(BlueprintImplementableEvent)
	void OnTrigger();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//protected functions
protected:

	//private functions
private:
};
