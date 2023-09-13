// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Duskborn/Character/BaseClasses/StateMachine.h"
#include "PlayerStateMachine.generated.h"

#define STATE_MACHINE Cast<UPlayerStateMachine>(StateMachine)

class UPlayerSprintState;
class UProsthesisBaseState;
class UPlayerJetPack;
class UPlayerDash;
class UPlayerGroundMovement;
class UPlayerGrappling;
class UPlayerGrapplingDash;
class APlayerCharacter;
class UPlayerBaseState;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DUSKBORN_API UPlayerStateMachine : public UStateMachine

{
	GENERATED_BODY()

	//-----------------------Variables----------------------------------------------------------------------------------  

	//public variables
public:
	UPROPERTY()
	APlayerCharacter* Player = nullptr;
	UPROPERTY()
	UPlayerGroundMovement* GroundMovementState = nullptr;
	UPROPERTY()
	UPlayerGrappling* GrapplingState = nullptr;
	UPROPERTY()
	UPlayerDash* DashState = nullptr;
	UPROPERTY()
	UPlayerGrapplingDash* GrapplingDashState = nullptr;
	UPROPERTY()
	UPlayerJetPack* JetPackState = nullptr;
	UPROPERTY()
	UPlayerSprintState* SprintState = nullptr;
	
	UPROPERTY()
	UProsthesisBaseState* CurrentCombatState = nullptr;
	//protected variables
protected:
	//private variables
private:
	//-----------------------Functions----------------------------------------------------------------------------------

	//public functions  
public:
	// Sets default values for this component's properties
	UPlayerStateMachine();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	/** Similar to normal state transition but for combat states from prostheses */
	void CombatStateTransition(UProsthesisBaseState* NewCombatState);

	/** Will set InCombat false which will prevent updating the current combat state */
	void DisableCombatState() const;
	/** Will set InCombat true which will allow updating the current combat state */
	void EnableCombatState() const;
	/** Will disable the current combat state and also end it*/
	void DeactivateCombatState() const;
	
	/** Will block player character movement which will prevent updating the current movement state */
	void DisableMovementState() const;
	/** Will unblock player character movement which will allow updating the current movement state */
	void EnableMovementState() const;
	/** Will disable the current combat state and also end it*/
	void DeactivateMovementState() const;

	virtual void Init() override;

	//protected functions
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//private functions  
private:
};
