// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputManager.h"
#include "Duskborn/Character/BaseClasses/BaseCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PlayerCharacter.generated.h"

class UBuffManagerComponent;
class UCombatNumberComponent;
class UTrigger;
class UEffectPart;
class USphereComponent;
class AEnemyCharacter;
class ARifleProsthesis;
class AProsthesis;
class UTriggerPart;
class UEffectPart;
class UItem;
enum class EInputs : uint8;
class UPlayerStateMachine;
class AGrapplingHook;
class ARifleProjectile;
class UStateMachine;
class UCameraComponent;
class USpringArmComponent;
class UCableComponent;
class UNiagaraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTriggerNotification, FString, Message);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStatChanged, int, Index, FString, Value);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FUpdateBars, int, Index, float, Min, float, Max);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryChanged);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FProsthesisSwapUI);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnemyHit);

UCLASS()
class DUSKBORN_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

	//-----------------------Variables----------------------------------------------------------------------------------  

	//public variables
public:
	UPROPERTY(BlueprintReadOnly)
	TArray<UItem*> ItemInventory;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Duskborn|Inventory")
	TArray<UEffectPart*> EffectPartsInventory;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Duskborn|Inventory")
	TArray<UTriggerPart*> TriggerPartsInventory;

	UPROPERTY()
	FTimerHandle DashCooldownTimer;
	UPROPERTY()
	FTimerHandle JetPackRefuelCooldownTimer;
	UPROPERTY()
	FTimerHandle NearbyEnemyTimer;
	UPROPERTY()
	FTimerHandle RefuelTimer;
	UPROPERTY()
	FTimerHandle LevelUpTimer;

	UPROPERTY(EditAnywhere, Category="Duskborn|Combat")
	TSubclassOf<AProsthesis> LeftProsthesisClass = nullptr;
	UPROPERTY(EditAnywhere, Category="Duskborn|Combat")
	TSubclassOf<AProsthesis> RightProsthesisClass = nullptr;

	UPROPERTY(BlueprintReadWrite)
	AProsthesis* LeftProsthesis = nullptr;
	UPROPERTY(BlueprintReadWrite)
	AProsthesis* RightProsthesis = nullptr;
	UPROPERTY(BlueprintReadWrite)
	AProsthesis* ActiveProsthesis = nullptr;
	UPROPERTY(BlueprintReadWrite)
	AProsthesis* InactiveProsthesis = nullptr;

	UPROPERTY(EditAnywhere)
	USpringArmComponent* SpringArm = nullptr;
	UPROPERTY(EditAnywhere)
	UCameraComponent* Camera = nullptr;
	UPROPERTY(EditAnywhere)
	USphereComponent* InteractionSphere = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBuffManagerComponent* BuffManager = nullptr;
	UPROPERTY(EditAnywhere)
	USceneComponent* BuffLocation = nullptr;

	UPROPERTY(BlueprintAssignable)
	FStatChanged StatChanged;
	UPROPERTY(BlueprintAssignable)
	FUpdateBars UpdateBars;
	UPROPERTY(BlueprintAssignable)
	FInventoryChanged InventoryChanged;
	UPROPERTY(BlueprintAssignable)
	FProsthesisSwapUI ProsthesisSwapUI;
	UPROPERTY(BlueprintAssignable)
	FEnemyHit EnemyHit;
	UPROPERTY(BlueprintAssignable)
	FTriggerNotification TriggerNotification;

	UPROPERTY()
	UPlayerStateMachine* StateMachine = nullptr;
	UPROPERTY(BlueprintReadOnly)
	UInputManager* InputManager = nullptr;
	UPROPERTY()
	AGrapplingHook* GrapplingHook = nullptr;

	FVector DashDirection = FVector::Zero();
	FVector LastHitLocation = FVector::Zero();

	/** Override Gravity Scale in Movement Component */
	UPROPERTY(EditAnywhere, Category="Duskborn|Movement")
	float GravityScale = 1.f;
	/** Override Max Acceleration in Movement Component */
	UPROPERTY(EditAnywhere, Category="Duskborn|Movement")
	float MaxAcceleration = 2048.f;
	/** Speed value for sprinting */
	UPROPERTY(EditAnywhere, Category="Duskborn|Movement|Ground")
	float SprintSpeed = 1800.f;
	/** Defines how much the character will slide over the ground during grappling. A higher value == more friction*/
	UPROPERTY(EditAnywhere, Category="Duskborn|Movement|Ground")
	float SlidingStrength = 0.1f;
	/** Override Friction in Movement Component */
	UPROPERTY(EditAnywhere, Category="Duskborn|Movement|Ground")
	float Friction = 8.0f;
	/** Override Max Fly Speed in Movement Component */
	UPROPERTY(EditAnywhere, Category="Duskborn|Movement|Air")
	float MaxFlySpeed = 1200.f;
	/** Amount of fuel for JetPack*/
	UPROPERTY(EditAnywhere, Category="Duskborn|Movement|Air", BlueprintReadOnly)
	float JetPackFuel = 100.f;
	UPROPERTY(BlueprintReadOnly)
	float CurrentJetPackFuel = JetPackFuel;
	/** Define amount of fuel depleted per second*/
	UPROPERTY(EditAnywhere, Category="Duskborn|Movement|Air")
	float FuelDepletionRate = 50.f;
	/** Define how much fuel is refueled per second*/
	UPROPERTY(EditAnywhere, Category="Duskborn|Movement|Air")
	float RefuelRate = 30.f;
	/** Define how fast the JetPack is */
	UPROPERTY(EditAnywhere, Category="Duskborn|Movement|Air")
	float JetPackStrength = 5000.f;
	/** Cooldown after which refueling will start */
	UPROPERTY(EditAnywhere, Category="Duskborn|Movement|Air")
	float JetPackRefuelCooldown = 1.f;
	/** Max time for Dash */
	UPROPERTY(EditAnywhere, Category="Duskborn|Movement|Dash")
	float DashDuration = 0.3f;
	float CurrentDashDuration = DashDuration;
	/** Dash cooldown */
	UPROPERTY(EditAnywhere, Category="Duskborn|Movement|Dash")
	float DashCooldown = 1.f;
	/** Define how fast the Dash is */
	UPROPERTY(EditAnywhere, Category="Duskborn|Movement|Dash")
	float DashStrength = 10000.f;
	float RopeOffset = 0.f;

	UPROPERTY(BlueprintReadOnly)
	int NearbyEnemyCount = 0;
	UPROPERTY(BlueprintReadWrite)
	int StrengthBuffCount = 0;
	UPROPERTY(BlueprintReadWrite)
	int CritBuffCount = 0;
	UPROPERTY(BlueprintReadWrite)
	int GrapplingSpeedBuffCount = 0;
	UPROPERTY(BlueprintReadWrite)
	int HealthRegenBuffCount = 0;
	UPROPERTY(BlueprintReadWrite)
	int JumpPowerBuffCount = 0;
	UPROPERTY(BlueprintReadWrite)
	int SpeedBuffCount = 0;
	int UpgradePoints = 0;

	UPROPERTY(EditAnywhere, Category="Duskborn|Sound")
	USoundBase* GettingHitSound = nullptr;
	UPROPERTY(EditAnywhere, Category="Duskborn|Sound")
	USoundBase* GrapplingHookHitSound = nullptr;
	UPROPERTY(EditAnywhere, Category="Duskborn|Sound")
	USoundBase* GrapplingHookPullingSound = nullptr;
	UPROPERTY(BlueprintReadOnly, Category="Duskborn|Sound")
	UAudioComponent* GrapplingHookPullingSoundAudioComponent = nullptr;
	UPROPERTY(EditAnywhere, Category="Duskborn|Sound")
	USoundBase* ThrusterSound = nullptr;
	UPROPERTY(BlueprintReadOnly, Category="Duskborn|Sound")
	UAudioComponent* ThrusterSoundAudioComponent = nullptr;
	UPROPERTY(EditAnywhere, Category="Duskborn|Sound")
	USoundBase* DashSound = nullptr;
	UPROPERTY(EditAnywhere, Category="Duskborn|Sound")
	USoundBase* LevelUpSound = nullptr;

	/** True if the player character is currently in jumping */
	UPROPERTY(BlueprintReadWrite)
	bool IsJumping = false;
	/** True if the player character is currently dashing */
	UPROPERTY(BlueprintReadOnly)
	bool IsDashing = false;
	/** True if the player character is currently sprinting */
	UPROPERTY(BlueprintReadOnly)
	bool IsSprinting = false;
	/** True if the player character is currently in combat (performing combat actions)*/
	UPROPERTY(BlueprintReadOnly)
	bool InCombat = false;
	/** True if the player character can grapple a surface */
	UPROPERTY(BlueprintReadOnly)
	bool CanGrapple = false;

	/** True if the player character is currently in grappling mode */
	bool IsGrappling = false;
	/** True if the player character gets pulled towards the grappling point */
	UPROPERTY(BlueprintReadOnly)
	bool IsPulling = false;
	/** True if PhysicsHook(BP_GrapplingHook) is currently used */
	bool UsingPhysicsHook = false;
	/** Allow the Rope to become shorter. For example after jumping while grappling the rope gets shorter */
	bool ShortenRope = false;
	/** True if player character is able to dash */
	bool CanDash = true;
	/** Will prevent update of CurrentState(movement state) in state machine if true */
	bool MovementBlocked = false;
	/** Prevent prosthesis swap if true */
	bool DisableProsthesisSwap = false;
	/** Cant use active prostheses primary attack if true */
	bool BlockPrimaryAttack = false;
	bool FreeCamBlocked = false;

	//protected variables
protected:
	/** Turn Aim Assist for grappling hook on and off */
	UPROPERTY(EditAnywhere, Category="Duskborn|Movement|GrapplingHook|AimAssist")
	bool TurnOnAimAssist = true;

	/** Reference of the grappling hook class */
	UPROPERTY(EditAnywhere, Category="Duskborn|Movement|GrapplingHook")
	TSubclassOf<class AGrapplingHook> GrapplingHookClass = nullptr;

	UPROPERTY(EditAnywhere)
	UCableComponent* Rope = nullptr;
	UPROPERTY(EditAnywhere)
	USphereComponent* EnemyDetectionSphere = nullptr;
	UPROPERTY(EditAnywhere)
	UNiagaraComponent* DashParticle = nullptr;

	/** Define how fast the rope gets pulled if sprint button is pressed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Duskborn|Movement|GrapplingHook")
	float PullSpeed = 600.f;

protected:
	/** Max distance for attaching the grappling hook */
	UPROPERTY(EditAnywhere, Category="Duskborn|Movement|GrapplingHook")
	float MaxGrappleDistance = 10000.f;
	/** Define how strong the character can swing back/forth and left/right while using the grappling hook */
	UPROPERTY(EditAnywhere, Category="Duskborn|Movement|GrapplingHook")
	float SwingSpeed = 400.f;
	/** Minimum radius of sphere trace for Aim Assist */
	UPROPERTY(EditAnywhere, Category="Duskborn|Movement|GrapplingHook|AimAssist")
	float MinAimAssistRadius = 10.f;
	/** Maximum radius of sphere trace for Aim Assist */
	UPROPERTY(EditAnywhere, Category="Duskborn|Movement|GrapplingHook|AimAssist")
	float MaxAimAssistRadius = 100.f;
	/** Defines the velocity for which the aim assist should apply the maximum radius */
	UPROPERTY(EditAnywhere, Category="Duskborn|Movement|GrapplingHook|AimAssist")
	float MaxVelocityForAimAssist = 5000.f;
	/** Defines min the Distance for Aim Assist. This is used to prevent snapping to a point close to the player */
	UPROPERTY(EditAnywhere, Category="Duskborn|Movement|GrapplingHook|AimAssist")
	float MinDistanceForAimAssist = 200.f;

	/** Override Arm Length for Camera */
	UPROPERTY(EditAnywhere, Category="Duskborn|Camera")
	float CameraArmLength = 800.0f;
	/** How fast the camera will zoom out when velocity is higher than Max Walk Speed */
	UPROPERTY(EditAnywhere, Category="Duskborn|Camera|Velocity Interpolation")
	float ZoomOutSpeed = 2.f;
	/** How fast the camera will zoom in when velocity is lower or equal to Max Walk Speed */
	UPROPERTY(EditAnywhere, Category="Duskborn|Camera|Velocity Interpolation")
	float ZoomInSpeed = 4.f;
	/** Max Camera Arm Length when zooming out */
	UPROPERTY(EditAnywhere, Category="Duskborn|Camera|Velocity Interpolation")
	float CameraMaxDistance = 1600.f;

	/** Amount of XP needed more for next level up */
	UPROPERTY(EditAnywhere, Category="Duskborn|LevelUP")
	float AddedXP = 100.f;
	/** Amount of Max HP added each level up */
	UPROPERTY(EditAnywhere, Category="Duskborn|LevelUP")
	float AddedMaxHP = 50.f;
	/** Amount of AttackDamage added each level up */
	UPROPERTY(EditAnywhere, Category="Duskborn|LevelUP")
	float AddedAttackDamage = 4.f;


	UPROPERTY(BlueprintReadOnly)
	float RopeLength = 0.f;

	/** Visualize Aim Assist in Editor */
	UPROPERTY(EditAnywhere, Category="Duskborn|Movement|GrapplingHook|AimAssist")
	TEnumAsByte<EDrawDebugTrace::Type> ShowAimAssist = EDrawDebugTrace::None;

	//private variables
private:
	UPROPERTY()
	UPrimitiveComponent* GrappledComponent = nullptr;

	FVector GrapplingPoint = FVector::Zero();

	UPROPERTY(EditAnywhere, Category="Duskborn|Stats")
	uint8 PlayerLevel = 0;
	UPROPERTY(EditAnywhere, Category="Duskborn|Stats")
	float MaxXP = 1000.f;
	float CurrentXP = 0.f;
	UPROPERTY(EditAnywhere, Category="Duskborn|Stats")
	float MaxEnergy = 30.f;
	float CurrentEnergy = 0.f;
	float CurrencyAmount = 0.f;

public:
	//-----------------------Functions----------------------------------------------------------------------------------

	//public functions  
	/** Sets default values for this character's properties */
	APlayerCharacter();

	/** Called to bind functionality to input */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Template to send inputs to InputManager. Needed because binding a function to action event can't use function inputs. */
	template <EInputs Input, EInputEvent Event>
	void HandleActionInput()
	{
		if (!InputManager) { return; }
		InputManager->ReceiveActionInput(Input, Event);
	};

	/** Template to send inputs to InputManager. Needed because binding a function to axis event can't use function inputs. */
	template <EInputs Input>
	void HandleAxisInput(const float AxisValue)
	{
		if (!InputManager) { return; }
		InputManager->ReceiveAxisInput(Input, AxisValue);
	};

	//Movement Functions

	/** Turn camera in horizontal direction */
	void TurnCamera(float AxisValue);

	/** Turn camera in vertical direction */
	void LookUp(float AxisValue);

	/** Move right while normally walking on the ground */
	void MoveRight(float AxisValue);

	/** Turn character with movement inputs. Needed for sprinting. */
	void TurnCharacter(float AxisValue);

	/** Use character rotation as forward vector to sprint in this direction. */
	void MoveForwardSprint(float AxisValue);

	/** Move character in forward vector direction */
	void MoveForward(float AxisValue);

	/** Add force to BP_GrapplingHook in right vector direction multiplied with axis value. */
	void SwingRight(float AxisValue) const;

	/** Add force to BP_GrapplingHook in forward vector direction multiplied with axis value. */
	void SwingForward(float AxisValue) const;

	/** Add force in right vector direction to player character. Needed for controls during pulling towards grappling point. */
	void ApplyForceRight(float AxisValue) const;

	/** Add force in up vector direction to player character. Needed for controls during pulling towards grappling point. */
	void ApplyForceUp(float AxisValue) const;

	void InitDash();
	void EnableDash();
	void ResetAfterDash();
	void InitJetPack();
	void EnableJetPack();
	void StartRefuelCooldown();
	void Refuel();
	void RefuelCallback();
	void StopRefuel();
	void StartSprinting();
	void StopSprinting();

	/** Used to trigger a dash by an effect */
	UFUNCTION(BlueprintCallable)
	void TriggerDash();
	virtual void Jump() override;

	//Functions for grappling

	/** Get point looked at and check if point can be grappled. Set point as new grappling point.
	 * @return True if point was found and set.
	 */
	bool FindGrapplingPoint();

	/** Check if point can be grappled. Used in tick to update visualization in UI.
	 * @return True if point can be grappled.
	 */
	bool CheckIfSurfaceCanBeGrappled() const;

	/** Attach end of cable component to grappling point */
	void AttachRopeToGrapplingPoint();

	void EnableRope() const;
	void DisableRope() const;

	/** Pull player character towards grappling point */
	void PullGrapplingHook();

	/** Update length between player character and grappling point */
	void CalcRopeLength();

	/** Setup BP_GrapplingHook and attach player character to it */
	void AttachPhysicsHook();
	/** Disable use of BP_GrapplingHook */
	void DetachPhysicsHook();
	void DetachGrapplingHook();

	/** Calculate the offset between RopeLength and the actual distance between grappling point and player character */
	void CalcRopeOffset();
	/** Shorten the rope of the grappling hook if distance between player and grappling point is shorter than rope length*/
	void ShortenRopeLength();

	void InitPullGrapplingHook();
	void StopPullGrapplingHook();

	/** Angle between player forward vector and the vector pointing into the direction of the grappling point */
	float GetAngleBetweenPlayerAndGrapplingPoint();

	//General Functions
	virtual bool Heal(float HealAmount) override;
	virtual void ReceiveDamage(float ReceivedDamage, bool Critical = false) override;

	void GetAmountOfEnemiesNearby();

	/** Use a sphere trace to detect a grappling point
	 * @return Hit Location
	 */
	FVector UseAimAssistForGrappling(FHitResult& Hit, float RayDistance = 10000.f);
	/** @return Direction in which the camara is currently pointing at */
	FVector GetAimVector() const;
	bool CheckIfSwinging() const;

	/** Handle how the spring arm length changes depending on the velocity */
	void HandleSpringArm(float DeltaTime) const;

	/** check if interactable actors are nearby and interact with them */
	void Interact();

	/** Camera will stop rotating the player character */
	void ActivateFreeCam();
	/** Camera will rotate the player character */
	void DeactivateFreeCam();

	/** @return point which is intersecting with a raycast in Viewport direction */
	UFUNCTION(BlueprintCallable)
	FVector GetPointedHit(FHitResult& Hit, FName TraceTag = TEXT("Visibility"), float RayDistance = 1000000.f) const;

	UFUNCTION(BlueprintCallable)
	void UpdateAllStatsForUI();

	virtual float DoDamageToCharacter(ABaseCharacter* OtherCharacter, float DamageScaler,
	                                  float AdditionalCriticalChance, float AdditionalCriticalMulti) override;
	UFUNCTION(BlueprintCallable)
	void FinishCurrentCombatState() const;

	/** Used in ABP_Player */
	UFUNCTION(BlueprintCallable)
	void NotifyCombatState();
	virtual void LevelUP(float Scalar = 1.f) override;
	virtual void OnCriticalDamage(float DamageAmount, ABaseCharacter* OtherCharacter) override;
	virtual void OnNormalDamage(float DamageAmount, ABaseCharacter* OtherCharacter) override;

	UFUNCTION(BlueprintImplementableEvent)
	void ScreenDistort();

	//Inventory
	UFUNCTION(BlueprintCallable)
	void EquipItem(UItem* Item);
	UFUNCTION(BlueprintCallable)
	void UnequipItem(UItem* Item);
	UFUNCTION(BlueprintCallable)
	void AddItem(UItem* Item);
	UFUNCTION(BlueprintCallable)
	void RemoveItem(UItem* Item);
	UFUNCTION(BlueprintCallable)
	void ScrapItem(UItem* Item);

	UFUNCTION(BlueprintCallable)
	void AddEffectPart(UEffectPart* Effect);
	UFUNCTION(BlueprintCallable)
	void RemoveEffectPart(UEffectPart* Effect);
	UFUNCTION(BlueprintCallable)
	void ScrapEffectPart(UEffectPart* Effect);

	UFUNCTION(BlueprintCallable)
	void AddTriggerPart(UTriggerPart* Trigger);
	UFUNCTION(BlueprintCallable)
	void RemoveTriggerPart(UTriggerPart* Trigger);
	UFUNCTION(BlueprintCallable)
	void ScrapTriggerPart(UTriggerPart* Trigger);

	UFUNCTION(BlueprintCallable)
	void AddCurrency(float currency);

	//Getter and Setter
	virtual void SetMaxHealth(float NewMaxHealth) override;
	virtual void SetCurrentHealth(float NewCurrentHealth) override;
	virtual void SetMovementSpeed(float NewMovementSpeed) override;
	virtual void SetAttackDamage(float NewAttackDamage) override;
	virtual void SetAttackSpeed(float NewAttackSpeed) override;
	virtual void SetCriticalStrikeChance(float NewCriticalStrikeChance) override;
	virtual void SetCriticalStrikeMultiplier(float NewCriticalStrikeMultiplier) override;
	virtual void SetCoolDownReduction(float NewSpecialCoolDownReduction) override;
	virtual void SetHealthRegeneration(float NewHealthRegeneration) override;

	UFUNCTION(BlueprintCallable)
	FVector GetGrapplingPoint();
	void SetGrapplingPoint(const FVector& NewGrapplingPoint, UPrimitiveComponent* NewGrappledComponent = nullptr);
	UFUNCTION(BlueprintCallable)
	uint8 GetPlayerLevel() const;
	UFUNCTION(BlueprintCallable)
	void SetPlayerLevel(uint8 NewLevel);
	UFUNCTION(BlueprintCallable)
	float GetMaxXP() const;
	UFUNCTION(BlueprintCallable)
	void SetMaxXP(float NewMaxXP);
	UFUNCTION(BlueprintCallable)
	float GetCurrentXP() const;
	UFUNCTION(BlueprintCallable)
	void SetCurrentXP(float NewCurrentXP);
	UFUNCTION(BlueprintCallable)
	float GetMaxEnergy() const;
	UFUNCTION(BlueprintCallable)
	void SetMaxEnergy(float NewMaxEnergy);
	UFUNCTION(BlueprintCallable)
	float GetCurrenEnergy() const;
	UFUNCTION(BlueprintCallable)
	void SetCurrentEnergy(float NewCurrentEnergy);
	UFUNCTION(BlueprintCallable)
	float GetCurrencyAmount() const;
	UFUNCTION(BlueprintCallable)
	void SetCurrencyAmount(float CurrencyAmount);
	UFUNCTION(BlueprintCallable)
	float GetSprintSpeed() const;
	UFUNCTION(BlueprintCallable)
	void SetSprintSpeed(float NewSprintSpeed);
	UFUNCTION(BlueprintCallable)
	int GetUpgradePoints() const;
	UFUNCTION(BlueprintCallable)
	void SetUpgradePoints(int NewUpgradePoints);
	UFUNCTION(BlueprintCallable)
	void SetPullSpeed(float NewPullSpeed);
	UFUNCTION(BlueprintCallable)
	float GetPullSpeed() const;

	float GetJetPackFuel() const;
	void SetJetPackFuel(float JetPackFuel);
	float GetCurrentJetPackFuel() const;
	void SetCurrentJetPackFuel(float CurrentJetPackFuel);

	//Functions for Prosthesis
	bool CreateProstheses();

	/** Change active prostheses to the currently deactivated one*/
	bool ProsthesisSwap();

	//Particles
	void ActivateDashParticle() const;
	void DeactivateDashParticle() const;
	UFUNCTION(BlueprintImplementableEvent)
	void ActivateThrusterParticle();
	UFUNCTION(BlueprintImplementableEvent)
	void DeactivateThrusterParticle();
	UFUNCTION(BlueprintImplementableEvent)
	void ActivateGrapplingParticle();
	UFUNCTION(BlueprintImplementableEvent)
	void DeactivateGrapplingParticle();
	UFUNCTION(BlueprintImplementableEvent)
	void ActivateLevelUpParticle();

	//Sounds
	void ActivateThrusterSound();
	void DeactivateThrusterSound() const;

	virtual void Die() override;

	//protected functions
protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	/** Called every frame */
	virtual void Tick(float DeltaTime) override;

	//private functions  
private:
};
