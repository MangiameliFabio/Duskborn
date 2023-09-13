// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Duskborn/Enums.h"
#include "Components/ActorComponent.h"
#include "InputManager.generated.h"

using FBufferTuple = TTuple<EInputs, EInputEvent, float>;

class APlayerCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DUSKBORN_API UInputManager : public UActorComponent
{
	GENERATED_BODY()

	//-----------------------Variables----------------------------------------------------------------------------------  

	//public variables
public:
	TArray<EInputs> PressedInputs;
	TArray<FBufferTuple> InputBuffer;

	UPROPERTY()
	APlayerCharacter* Player;

	FVector LastInputValue = FVector::Zero();
	FVector CurrentMovementInput = FVector::Zero();

	float BufferDuration = 1.f;

	//protected variables
protected:
	//private variables
private:
	int InputBufferMaxLength = 10.f;
	int BufferPosition = 0.f;
	bool HandleInputBuffer = false;
	//-----------------------Functions----------------------------------------------------------------------------------

	//public functions  
public:
	UInputManager(): UActorComponent(){}

	/** Send action input to current movement and combat state */
	void SendActionInput(EInputs Input, EInputEvent Event,const bool FromBuffer = false);

	/** Send axis input to current movement and combat state */
	void SendAxisInput(EInputs Input, float AxisValue);

	/** Get action inputs from player character */
	void ReceiveActionInput(EInputs Input, EInputEvent Event);

	/** Get axis inputs from player character */
	void ReceiveAxisInput(EInputs Input, float AxisValue);

	/** Use inputs from the last tick and normalize them to get input direction */
	void UpdateLastInputDirection();

	/** Check if specific input is currently pressed */
	bool CheckIfInputIsPressed(EInputs Input);

	/** Add input to InputBuffer */
	void AddToInputBuffer(EInputs Input, EInputEvent Event);

	/** Check if InputBuffer has content */
	void CheckInputBuffer();

	/** Handle buffered input */
	void HandleBufferedInput(FBufferTuple* Buffered);
	void ClearInputBuffer();

	UFUNCTION(BlueprintCallable)
	FVector GetLastInputDirection();
	UFUNCTION(BlueprintCallable)
	void ClearCurrentInputs();

	//protected functions
protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	//private functions  
private:
};
