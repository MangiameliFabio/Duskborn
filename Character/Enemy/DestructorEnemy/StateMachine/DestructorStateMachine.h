// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Duskborn/Character/Enemy/StateMachine/EnemyStateMachine.h"
#include "DestructorStateMachine.generated.h"

#define DESTRUCTOR_SM Cast<UDestructorStateMachine>(StateMachine)
#define DESTRUCTOR Cast<UDestructorStateMachine>(StateMachine)->Destructor

class ADestructorEnemy;
class UDestructorActive;
class UDestructorIdle;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DUSKBORN_API UDestructorStateMachine : public UEnemyStateMachine
{
	GENERATED_BODY()

	//-----------------------Variables----------------------------------------------------------------------------------  

	//public variables
public:

	//States
	UPROPERTY()
	UDestructorIdle* IdleState = nullptr;
	UPROPERTY()
	UDestructorActive* ActiveState = nullptr;

	//Self reference
	UPROPERTY()
	ADestructorEnemy* Destructor = nullptr;
	
	//protected variables
protected:
	//private variables
private:
	bool DoOnce = true;
	//-----------------------Functions----------------------------------------------------------------------------------

	//public functions  
public:
	UDestructorStateMachine();

	//protected functions
protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	//private functions  
private:
};
