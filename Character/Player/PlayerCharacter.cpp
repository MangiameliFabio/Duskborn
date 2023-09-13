// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "BuffManagerComponent.h"
#include "CableComponent.h"
#include "GrapplingHook.h"
#include "InputManager.h"
#include "NiagaraComponent.h"
#include "Duskborn/DuskbornGameModeBase.h"
#include "Duskborn/Enums.h"
#include "Duskborn/Character/BaseClasses/State.h"
#include "Duskborn/Character/Enemy/EnemyCharacter.h"
#include "Duskborn/Environment/Interactable.h"
#include "Duskborn/Helper/Log.h"
#include "Duskborn/Management/WorldManager.h"
#include "Duskborn/UI/CombatNumberComponent.h"
#include "Duskborn/UpgradeSystem/TriggerManager.h"
#include "Duskborn/UpgradeSystem/BaseClasses/Effect.h"
#include "Duskborn/UpgradeSystem/BaseClasses/EffectPart.h"
#include "Duskborn/UpgradeSystem/BaseClasses/Item.h"
#include "Duskborn/UpgradeSystem/BaseClasses/Trigger.h"
#include "Duskborn/UpgradeSystem/BaseClasses/TriggerPart.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "RifleProsthesis/RifleProsthesis.h"
#include "StateMachine/PlayerStateMachine.h"
#include "StateMachine/PlayerStates.h"


APlayerCharacter::APlayerCharacter() : ABaseCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StateMachine = CreateDefaultSubobject<UPlayerStateMachine>("State Machine");
	InputManager = CreateDefaultSubobject<UInputManager>("InputManager");
	BuffManager = CreateDefaultSubobject<UBuffManagerComponent>("BuffManager");

	Rope = CreateDefaultSubobject<UCableComponent>("Rope");
	Rope->SetupAttachment(GetMesh(), "GrapplingSocket");

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);

	DashParticle = CreateDefaultSubobject<UNiagaraComponent>("DashParticle");
	DashParticle->SetupAttachment(RootComponent);

	EnemyDetectionSphere = CreateDefaultSubobject<USphereComponent>("EnemyDetection");
	EnemyDetectionSphere->SetupAttachment(RootComponent);

	InteractionSphere = CreateDefaultSubobject<USphereComponent>("InteractionSphere");
	InteractionSphere->SetupAttachment(RootComponent);

	BuffLocation = CreateDefaultSubobject<USceneComponent>("BuffLocation");
	BuffLocation->SetupAttachment(RootComponent);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Spawn BP_GrapplingHook in level to use as physics hook
	if (!GrapplingHook)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		GrapplingHook = GetWorld()->SpawnActor<AGrapplingHook>(GrapplingHookClass, GetActorLocation(),
		                                                       GetActorRotation(), SpawnParams);
	}

	if (!GetCharacterMovement())
	{
		Log::Print("No Movement Component found on Player Character");
		return;
	}
	//Override MovementComponent Stats
	GetCharacterMovement()->GravityScale = GravityScale;
	GetCharacterMovement()->MaxAcceleration = MaxAcceleration;
	GetCharacterMovement()->MaxFlySpeed = MaxFlySpeed;
	GetCharacterMovement()->GroundFriction = Friction;

	if (!SpringArm)
	{
		Log::Print("No SpringArm found on Player Character");
		return;
	}
	SpringArm->TargetArmLength = CameraArmLength;

	if (CreateProstheses())
	{
		//Initialize state machine when prostheses are in the level
		StateMachine->Init();
	}

	GetWorldTimerManager().SetTimer(NearbyEnemyTimer, this, &APlayerCharacter::GetAmountOfEnemiesNearby, 1.f, true);

	Cast<ADuskbornGameModeBase>(GetWorld()->GetAuthGameMode())->InitUI();
}

void APlayerCharacter::Tick(const float DeltaTime)
{
	check(GetWorld())
	if (!IsDead)
	{
		Super::Tick(DeltaTime);

		//This will update the visualization on the cross hair 
		CanGrapple = CheckIfSurfaceCanBeGrappled();

		//Conditions when camera can be moved freely around the player
		if (GetCharacterMovement()->IsMovingOnGround() && GetCharacterMovement()->Velocity.Length() == 0.f && !
			InCombat)
		{
			if (!FreeCamBlocked)
			{
				ActivateFreeCam();
			}
		}
		else if (IsSprinting && GetCharacterMovement()->IsMovingOnGround())
		{
			ActivateFreeCam();
		}
		else
		{
			DeactivateFreeCam();
		}

		//Reset character friction if speed is smaller than MaxWalkSpeed. During grappling the friction is lowered to make drifting possible.
		if (GetCharacterMovement()->Velocity.Length() <= GetCharacterMovement()->MaxWalkSpeed)
		{
			GetCharacterMovement()->GroundFriction = Friction;
		}

		HandleSpringArm(DeltaTime);

		//Notify items subscribed to trigger "while full hp"
		//Notify items subscribed to trigger "while hp below value"
		if (!TRIGGER_MANAGER)
		{
			Log::Print("No Trigger Manager found in Player");
			return;
		}
		if (CurrentHealth >= MaxHealth)
			TRIGGER_MANAGER->ActivateTrigger(ETriggerTypes::WHILE_FULL_HP);
		else
			TRIGGER_MANAGER->DeactivateTrigger(ETriggerTypes::WHILE_FULL_HP);

		if (TRIGGER_MANAGER->FindTrigger(ETriggerTypes::HP_BELOW_VALUE) && CurrentHealth <= MaxHealth * (
			TRIGGER_MANAGER->FindTrigger(ETriggerTypes::HP_BELOW_VALUE)->ThreshHold /
			100))
		{
			TRIGGER_MANAGER->ActivateTrigger(ETriggerTypes::HP_BELOW_VALUE);
		}
		else
		{
			TRIGGER_MANAGER->DeactivateTrigger(ETriggerTypes::HP_BELOW_VALUE);
		}

		//Trigger "while standing" and "while grounded" are currently not in use
		
		// if (!GetCharacterMovement()->IsMovingOnGround())
		// {
		// 	TRIGGER_MANAGER->DeactivateTrigger(ETriggerTypes::WHILE_GROUNDED);
		// 	GetCharacterMovement()->GroundFriction = SlidingStrength;
		// }
		// else
		// {
		// 	if (GetCharacterMovement()->Velocity.Length() <= 0.f)
		// 	{
		// 		TRIGGER_MANAGER->ActivateTrigger(ETriggerTypes::WHILE_STANDING);
		// 	}
		// 	else
		// 	{
		// 		TRIGGER_MANAGER->DeactivateTrigger(ETriggerTypes::WHILE_STANDING);
		// 	}
		// 	TRIGGER_MANAGER->ActivateTrigger(ETriggerTypes::WHILE_GROUNDED);
		// }
	}
}

void APlayerCharacter::SetMaxHealth(const float NewMaxHealth)
{
	Super::SetMaxHealth(NewMaxHealth);

	//Broadcast which will update UI
	StatChanged.Broadcast(
		1, FString::FromInt(FMath::RoundToInt(GetCurrentHealth())) + "/" + FString::FromInt(
			FMath::RoundToInt(NewMaxHealth)));
	UpdateBars.Broadcast(1, FMath::RoundToInt(GetCurrentHealth()), FMath::RoundToInt(NewMaxHealth));
}

void APlayerCharacter::SetCurrentHealth(const float NewCurrentHealth)
{
	Super::SetCurrentHealth(NewCurrentHealth);

	//Broadcast which will update UI
	StatChanged.Broadcast(
		1, FString::FromInt(FMath::RoundToInt(NewCurrentHealth)) + "/" + FString::FromInt(
			FMath::RoundToInt(GetMaxHealth())));
	UpdateBars.Broadcast(1, FMath::RoundToInt(NewCurrentHealth), FMath::RoundToInt(GetMaxHealth()));
}

void APlayerCharacter::SetMovementSpeed(const float NewMovementSpeed)
{
	Super::SetMovementSpeed(NewMovementSpeed);

	//Broadcast which will update UI
	StatChanged.Broadcast(14, FString::FromInt(NewMovementSpeed));
}

void APlayerCharacter::SetAttackDamage(const float NewAttackDamage)
{
	Super::SetAttackDamage(NewAttackDamage);

	//Broadcast which will update UI
	StatChanged.Broadcast(3, FString::FromInt(NewAttackDamage));
}

void APlayerCharacter::SetAttackSpeed(const float NewAttackSpeed)
{
	Super::SetAttackSpeed(NewAttackSpeed);

	//Broadcast which will update UI
	StatChanged.Broadcast(4, FString::FromInt(NewAttackSpeed) + "%");
}

void APlayerCharacter::SetCriticalStrikeChance(const float NewCriticalStrikeChance)
{
	Super::SetCriticalStrikeChance(NewCriticalStrikeChance);

	//Broadcast which will update UI
	if (NewCriticalStrikeChance < 0)
	{
		StatChanged.Broadcast(8, "-" + FString::FromInt(NewCriticalStrikeChance) + "%");
	}
	else
		StatChanged.Broadcast(8, "+" + FString::FromInt(NewCriticalStrikeChance) + "%");
}

void APlayerCharacter::SetCriticalStrikeMultiplier(const float NewCriticalStrikeMultiplier)
{
	Super::SetCriticalStrikeMultiplier(NewCriticalStrikeMultiplier);

	//Broadcast which will update UI
	if (NewCriticalStrikeMultiplier < 0)
	{
		StatChanged.Broadcast(7, "-" + FString::FromInt((NewCriticalStrikeMultiplier - 1) * 100) + "%");
	}
	else
		StatChanged.Broadcast(7, "+" + FString::FromInt((NewCriticalStrikeMultiplier - 1) * 100) + "%");
}

void APlayerCharacter::SetCoolDownReduction(float NewSpecialCoolDownReduction)
{
	Super::SetCoolDownReduction(NewSpecialCoolDownReduction);

	StatChanged.Broadcast(9, FString::FromInt(NewSpecialCoolDownReduction) + "%");
}

void APlayerCharacter::SetHealthRegeneration(float NewHealthRegeneration)
{
	Super::SetHealthRegeneration(NewHealthRegeneration);

	StatChanged.Broadcast(15, FString::FromInt(NewHealthRegeneration) + " per sec.");
}

uint8 APlayerCharacter::GetPlayerLevel() const
{
	return PlayerLevel;
}

void APlayerCharacter::SetPlayerLevel(const uint8 NewPlayerLevel)
{
	this->PlayerLevel = NewPlayerLevel;

	//Broadcast which will update UI
	StatChanged.Broadcast(0, FString::FromInt(NewPlayerLevel));
}

float APlayerCharacter::GetMaxXP() const
{
	return MaxXP;
}

void APlayerCharacter::SetMaxXP(const float NewMaxXP)
{
	this->MaxXP = NewMaxXP;

	//Broadcast which will update UI
	UpdateBars.Broadcast(0, GetCurrentXP(), NewMaxXP);
}

float APlayerCharacter::GetCurrentXP() const
{
	return CurrentXP;
}

void APlayerCharacter::SetCurrentXP(const float NewCurrentXP)
{
	this->CurrentXP = NewCurrentXP;

	//Broadcast which will update UI
	UpdateBars.Broadcast(0, NewCurrentXP, GetMaxXP());
}

float APlayerCharacter::GetMaxEnergy() const
{
	return MaxEnergy;
}

void APlayerCharacter::SetMaxEnergy(const float NewMaxEnergy)
{
	this->MaxEnergy = NewMaxEnergy;

	//Broadcast which will update UI
	StatChanged.Broadcast(2, FString::FromInt(GetCurrenEnergy()) + "/" + FString::FromInt(NewMaxEnergy));
}

float APlayerCharacter::GetCurrenEnergy() const
{
	return CurrentEnergy;
}

void APlayerCharacter::SetCurrentEnergy(const float NewCurrentEnergy)
{
	this->CurrentEnergy = NewCurrentEnergy;

	//Broadcast which will update UI
	StatChanged.Broadcast(2, FString::FromInt(NewCurrentEnergy) + "/" + FString::FromInt(GetMaxEnergy()));
}

float APlayerCharacter::GetCurrencyAmount() const
{
	return CurrencyAmount;
}

void APlayerCharacter::SetCurrencyAmount(const float NewCurrencyAmount)
{
	this->CurrencyAmount = NewCurrencyAmount;

	//Broadcast which will update UI
	StatChanged.Broadcast(10, FString::FromInt(NewCurrencyAmount));
}

float APlayerCharacter::GetSprintSpeed() const
{
	return SprintSpeed;
}

void APlayerCharacter::SetSprintSpeed(const float NewSprintSpeed)
{
	this->SprintSpeed = NewSprintSpeed;
}

float APlayerCharacter::GetJetPackFuel() const
{
	return JetPackFuel;
}

void APlayerCharacter::SetJetPackFuel(const float NewJetPackFuel)
{
	this->JetPackFuel = NewJetPackFuel;
}

float APlayerCharacter::GetCurrentJetPackFuel() const
{
	return CurrentJetPackFuel;
}

void APlayerCharacter::SetCurrentJetPackFuel(const float NewCurrentJetPackFuel)
{
	this->CurrentJetPackFuel = NewCurrentJetPackFuel;
}

int APlayerCharacter::GetUpgradePoints() const
{
	return UpgradePoints;
}

void APlayerCharacter::SetUpgradePoints(int NewUpgradePoints)
{
	this->UpgradePoints = NewUpgradePoints;

	//Broadcast which will update UI
	StatChanged.Broadcast(12, "Enhancments: " + FString::FromInt(NewUpgradePoints));
}

void APlayerCharacter::SetPullSpeed(float NewPullSpeed)
{
	this->PullSpeed = NewPullSpeed;

	//Broadcast which will update UI
	StatChanged.Broadcast(13, FString::FromInt(NewPullSpeed));
}

float APlayerCharacter::GetPullSpeed() const
{
	return PullSpeed;
}

FVector APlayerCharacter::GetGrapplingPoint()
{
	//Check if grappled component is valid
	if (GrappledComponent)
	{
		if (const AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GrappledComponent->GetOwner()))
		{
			//If enemy is dead release grappling hook and clear pointer GrappledComponent
			if (Enemy->IsDead)
			{
				GrappledComponent = nullptr;
				DetachGrapplingHook();
				StopPullGrapplingHook();
				StateMachine->StateTransition(StateMachine->GroundMovementState);
			}
			else
			//Return location of the component as grappling point
				return GrappledComponent->GetComponentTransform().GetLocation();
		}
	}

	//Return saved grappling point
	return GrapplingPoint;
}

void APlayerCharacter::SetGrapplingPoint(const FVector& NewGrapplingPoint, UPrimitiveComponent* NewGrappledComponent)
{
	//If NewGrappledComponent is valid and the owning enemy is not dead save it
	if (NewGrappledComponent)
	{
		if (const ABaseCharacter* Character = Cast<ABaseCharacter>(NewGrappledComponent->GetOwner()))
		{
			if (Character->IsDead)
			{
				GrappledComponent = nullptr;
			}
			else
			{
				GrappledComponent = NewGrappledComponent;
			}
		}
	}
	else
	{
		GrappledComponent = nullptr;
	}

	//Save new grappling point
	GrapplingPoint = NewGrapplingPoint;
}

void APlayerCharacter::TurnCamera(const float AxisValue)
{
	AddControllerYawInput(AxisValue);
}

void APlayerCharacter::LookUp(const float AxisValue)
{
	AddControllerPitchInput(AxisValue);
}

void APlayerCharacter::MoveRight(const float AxisValue)
{
	const FVector Dir = GetActorRightVector() * AxisValue;
	AddMovementInput(Dir);
}

void APlayerCharacter::TurnCharacter(float AxisValue)
{
	const FRotator Rotation(0, GetControlRotation().Yaw, 0);
	AddMovementInput(UKismetMathLibrary::GetRightVector(Rotation), AxisValue);
}

void APlayerCharacter::MoveForwardSprint(float AxisValue)
{
	const FRotator Rotation(0, GetControlRotation().Yaw, 0);
	AddMovementInput(UKismetMathLibrary::GetForwardVector(Rotation), AxisValue);
}

void APlayerCharacter::MoveForward(const float AxisValue)
{
	AddMovementInput(GetActorForwardVector(), AxisValue);
}

void APlayerCharacter::SwingRight(const float AxisValue) const
{
	const FVector SwingForce = GetActorRightVector() * AxisValue * SwingSpeed * 100.f;

	if (!GrapplingHook)
	{
		Log::Print("No Grappling Hook found in Player");
		return;
	}
	GrapplingHook->AddSwingForce(SwingForce);
}

void APlayerCharacter::SwingForward(const float AxisValue) const
{
	const FVector SwingForce = GetActorForwardVector() * AxisValue * SwingSpeed * 100.f;
	if (!GrapplingHook)
	{
		Log::Print("No Grappling Hook found in Player");
		return;
	}
	GrapplingHook->AddSwingForce(SwingForce);
}

void APlayerCharacter::ApplyForceRight(float AxisValue) const
{
	GetCharacterMovement()->AddForce(GetActorRightVector() * AxisValue * PullSpeed * 500.f);
}

void APlayerCharacter::ApplyForceUp(float AxisValue) const
{
	GetCharacterMovement()->AddForce(GetActorUpVector() * AxisValue * PullSpeed * 500.f);
}

void APlayerCharacter::InitDash()
{
	check(GetWorld())
	GetCharacterMovement()->Velocity = FVector::Zero();

	//Prevent dashing before cooldown is finished
	CanDash = false;

	//Setup Cooldown Timer for Dash
	if (DashCooldown > 0.f)
	{
		GetWorldTimerManager().SetTimer(DashCooldownTimer, this, &APlayerCharacter::EnableDash,
		                                DashCooldown);
	}
	else
	{
		EnableDash();
	}

	//Reset Dash Duration
	CurrentDashDuration = DashDuration;

	if (InputManager->CheckIfInputIsPressed(EInputs::SPRINT) && GetCharacterMovement()->IsWalking())
		DashDirection = PLAYER->GetActorForwardVector();

	else
		//Use last InputVector as DashDirection and remove down velocity
		DashDirection = InputManager->GetLastInputDirection();

	DashDirection.Z = 0;

	//Use Forward Vector if no input is pressed. Player will dash forward while standing still
	if (DashDirection == FVector::Zero())
	{
		DashDirection = GetActorForwardVector();
	}

	ActivateDashParticle();
	if (DashSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DashSound, GetActorLocation());
	}

	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCharacterMovement()->MaxFlySpeed = DashStrength;
	GetCharacterMovement()->MaxAcceleration = 100000.f; //Needs a high value for instant velocity change

	IsDashing = true;
	IsJumping = false;
}

void APlayerCharacter::EnableDash()
{
	check(GetWorld())
	GetWorldTimerManager().ClearTimer(DashCooldownTimer);
	CanDash = true;
}

void APlayerCharacter::ResetAfterDash()
{
	//Check if action is still pressed to enable combat state
	if (InputManager->CheckIfInputIsPressed(EInputs::ACTION) && !BlockPrimaryAttack)
		StateMachine->EnableCombatState();

	//Reset movement component to normal
	GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	GetCharacterMovement()->MaxFlySpeed = MaxFlySpeed;
	GetCharacterMovement()->MaxAcceleration = MaxAcceleration;
	GetCharacterMovement()->Velocity = DashDirection * GetCharacterMovement()->MaxWalkSpeed;

	DeactivateDashParticle();
	IsDashing = false;
}

void APlayerCharacter::InitJetPack()
{
	check(GetWorld())

	//Stop refueling the jetpack tank
	StopRefuel();

	//Clear timer after which refueling should start
	GetWorldTimerManager().ClearTimer(JetPackRefuelCooldownTimer);

	//Set X and Y of the velocity to 0 to stop the player character 
	GetCharacterMovement()->Velocity.Y = 0.f;
	GetCharacterMovement()->Velocity.X = 0.f;
	if (GetCharacterMovement()->Velocity.Z < 0.f)
	{
		//Set Z of velocity 0 if its below 0
		GetCharacterMovement()->Velocity.Z = 0.f;
	}
	else if (GetCharacterMovement()->Velocity.Z > GetCharacterMovement()->JumpZVelocity)
	{
		//Clamp Z velocity to max jump velocity. This feels much smoother when activating the jetpack directly after jumping
		GetCharacterMovement()->Velocity.Z = GetCharacterMovement()->JumpZVelocity;
	}

	//Setup movement component
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCharacterMovement()->MaxFlySpeed = JetPackStrength;

	ActivateThrusterParticle();
	ActivateThrusterSound();
}

void APlayerCharacter::EnableJetPack()
{
	if (!GetWorld())
	{
		Log::Print("No World found");
		return;
	}
	GetWorldTimerManager().ClearTimer(JetPackRefuelCooldownTimer);
}

void APlayerCharacter::StartRefuelCooldown()
{
	if (!GetWorld())
	{
		Log::Print("No World found");
		return;
	}
	GetWorldTimerManager().SetTimer(JetPackRefuelCooldownTimer, this, &APlayerCharacter::Refuel,
	                                JetPackRefuelCooldown);
}

void APlayerCharacter::Refuel()
{
	if (!GetWorld())
	{
		Log::Print("No World found");
		return;
	}

	GetWorldTimerManager().SetTimer(RefuelTimer, [this]()
	{
		RefuelCallback();
	}, GetWorld()->DeltaTimeSeconds, true);
}

void APlayerCharacter::RefuelCallback()
{
	if (!GetWorld())
	{
		Log::Print("No World found");
		return;
	}

	//Check if tank is full and stop refuel if true
	if (GetCurrentJetPackFuel() > GetJetPackFuel())
	{
		SetCurrentJetPackFuel(GetJetPackFuel());
		StopRefuel();
	}
	else
	{
		//Refuel tank
		SetCurrentJetPackFuel(GetCurrentJetPackFuel() + GetWorld()->DeltaTimeSeconds * RefuelRate);
	}
}

void APlayerCharacter::StopRefuel()
{
	GetWorldTimerManager().ClearTimer(RefuelTimer);
}

void APlayerCharacter::StartSprinting()
{
	IsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

	//Prevent performing combat actions during sprinting
	STATE_MACHINE->DisableCombatState();

	check(GetWorld())
	if (!TRIGGER_MANAGER)
	{
		Log::Print("No Trigger Manager found");
		return;
	}

	//Notify all items subscribed to while sprinting trigger
	TRIGGER_MANAGER->ActivateTrigger(ETriggerTypes::WHILE_SPRINTING);
}

void APlayerCharacter::StopSprinting()
{
	IsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;

	check(GetWorld())
	if (!TRIGGER_MANAGER)
	{
		Log::Print("No Trigger Manager found");
		return;
	}
	//Stop notifying items subscribed to while sprinting trigger
	TRIGGER_MANAGER->DeactivateTrigger(ETriggerTypes::WHILE_SPRINTING);
}

void APlayerCharacter::TriggerDash()
{
	//Choose which dash state should be used
	if (IsGrappling)
		StateMachine->StateTransition(StateMachine->GrapplingDashState);
	else
		StateMachine->StateTransition(StateMachine->DashState);

	//Allow dashing. A dash triggered by an effect shouldn't set the dash on cooldown
	CanDash = true;

	GetWorldTimerManager().ClearTimer(DashCooldownTimer);
}

bool APlayerCharacter::FindGrapplingPoint()
{
	FHitResult Hit;

	//Find point for grappling hook
	FVector PointedHitLocation = GetPointedHit(Hit, FName(TEXT("GrapplingHookTrace")), MaxGrappleDistance);

	//Set grappling point
	if (Hit.bBlockingHit)
	{
		if (GrapplingHookHitSound)
			UGameplayStatics::PlaySound2D(this, GrapplingHookHitSound);

		if (Cast<AEnemyCharacter>(Hit.GetActor()))
			SetGrapplingPoint(PointedHitLocation, Hit.GetComponent());
		else
			SetGrapplingPoint(PointedHitLocation);

		return true;
	}

	//If the normal check did not find a point try again with aim assist
	if (TurnOnAimAssist)
	{
		PointedHitLocation = UseAimAssistForGrappling(Hit, MaxGrappleDistance);
		if (Hit.bBlockingHit)
		{
			if (FVector::Dist(PointedHitLocation, GetActorLocation()) > MinDistanceForAimAssist)
			{
				if (GrapplingHookHitSound)
					UGameplayStatics::PlaySound2D(this, GrapplingHookHitSound);
				if (Cast<AEnemyCharacter>(Hit.GetActor()))
					SetGrapplingPoint(PointedHitLocation, Hit.GetComponent());
				else
					SetGrapplingPoint(PointedHitLocation);

				return true;
			}
		}
	}
	return false;
}

bool APlayerCharacter::CheckIfSurfaceCanBeGrappled() const
{
	FHitResult Hit;
	GetPointedHit(Hit, FName(TEXT("GrapplingHookTrace")), MaxGrappleDistance);

	if (Hit.bBlockingHit)
	{
		return true;
	}
	return false;
}

void APlayerCharacter::AttachRopeToGrapplingPoint()
{
	const FTransform InverseTransform = GetTransform().Inverse();
	const FVector InverseLocation = InverseTransform.TransformPosition(GetGrapplingPoint());

	Rope->EndLocation = InverseLocation;
}

void APlayerCharacter::EnableRope() const
{
	Rope->SetVisibility(true);
}

void APlayerCharacter::DisableRope() const
{
	Rope->SetVisibility(false);
}

void APlayerCharacter::CalcRopeLength()
{
	RopeLength = FVector::Dist(GetActorLocation(), GetGrapplingPoint());
}

void APlayerCharacter::AttachPhysicsHook()
{
	if (!UsingPhysicsHook)
	{
		if (!GrapplingHook)
		{
			Log::Print("No Grappling Hook reference found in Player");
			return;
		}

		GrapplingHook->SetupGrapplingHook(GetGrapplingPoint(), GetActorLocation(), GetCharacterMovement()->Velocity,
		                                  GrappledComponent);
		UsingPhysicsHook = true;
		ShortenRope = false;
	}
}

void APlayerCharacter::DetachPhysicsHook()
{
	if (!GrapplingHook)
	{
		Log::Print("No Grappling Hook reference found in Player");
		return;
	}
	GrapplingHook->DetachGrapplingHook();
	UsingPhysicsHook = false;
}

void APlayerCharacter::DetachGrapplingHook()
{
	if (GetCharacterMovement()->IsFlying())
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);

	GrappledComponent = nullptr;

	IsGrappling = false;
	DisableRope();
}

void APlayerCharacter::CalcRopeOffset()
{
	RopeOffset = FVector::Dist(GetGrapplingPoint(), GetActorLocation()) - RopeLength;
}

void APlayerCharacter::ShortenRopeLength()
{
	const float NewRopeLength = FVector::Dist(GetGrapplingPoint(), GetActorLocation());
	if (NewRopeLength <= RopeLength)
	{
		RopeLength = NewRopeLength;
	};
}

void APlayerCharacter::PullGrapplingHook()
{
	if (GetCharacterMovement()->MovementMode != MOVE_Flying)
	{
		//Change ground friction of the player character. This makes pulling while being on the ground possible.
		GetCharacterMovement()->GroundFriction = SlidingStrength;

		//If angle is to high go into fly mode. This will let the player character move on a path directly to the grappling point.
		//Without this, the character would not take off and be pulled further over the ground
		if (GetAngleBetweenPlayerAndGrapplingPoint() >= 10.f)
		{
			GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		}
	}

	//Calculate direction for velocity change
	FVector VelocityDir = GetGrapplingPoint() - GetActorLocation();
	VelocityDir.Normalize();

	//Iterploate character velocity
	const FVector InterpolatedVelocity = FMath::VInterpTo(GetCharacterMovement()->Velocity,
	                                                      VelocityDir * PullSpeed * 20.f,
	                                                      GetWorld()->GetDeltaSeconds(), 1);
	GetCharacterMovement()->Velocity = InterpolatedVelocity;
}

void APlayerCharacter::InitPullGrapplingHook()
{
	if (!IsPulling)
	{
		IsPulling = true;
		ShortenRope = true;
		ScreenDistort(); //Not used
		if (GrapplingHookPullingSound)
		{
			GrapplingHookPullingSoundAudioComponent = UGameplayStatics::SpawnSound2D(this, GrapplingHookPullingSound);
		}
	}
}

void APlayerCharacter::StopPullGrapplingHook()
{
	IsPulling = false;
	if (GetCharacterMovement()->IsFlying())
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);

	if (GrapplingHookPullingSoundAudioComponent)
	{
		GrapplingHookPullingSoundAudioComponent->Deactivate();
	}
}

float APlayerCharacter::GetAngleBetweenPlayerAndGrapplingPoint()
{
	//Get direction vector from player to grappling point
	const FVector DirToGrapplingPoint = GetGrapplingPoint() - GetActorLocation();

	//Set grappling point Z value to player Z value
	const FVector Horizontal(GetGrapplingPoint().X, GetGrapplingPoint().Y, GetActorLocation().Z);
	const FVector HorizontalDir = Horizontal - GetActorLocation();

	//Calculate Dot Product
	const float DotProduct = FVector::DotProduct(HorizontalDir, DirToGrapplingPoint);

	// Calculate the magnitudes of the two vectors
	const float MagnitudeA = HorizontalDir.Length();
	const float MagnitudeB = DirToGrapplingPoint.Length();

	return UKismetMathLibrary::DegAcos(DotProduct / (MagnitudeA * MagnitudeB));
}

bool APlayerCharacter::Heal(float HealAmount)
{
	if (Super::Heal(HealAmount))
	{
		if (!GetWorld())
		{
			Log::Print("World not found");
			return false;
		}
		
		//Trigger "on healing" is currently not in use
		
		// if (TRIGGER_MANAGER)
		// {
		// 	//Notify all items subscribed to on healing trigger
		// 	TRIGGER_MANAGER->Notify(this, ETriggerTypes::ON_HEALING);
		// }
		// else { Log::Print("No Trigger Manager found"); }
	};

	return true;
}

void APlayerCharacter::ReceiveDamage(const float ReceivedDamage, bool Critical)
{
	Super::ReceiveDamage(ReceivedDamage);

	UGameplayStatics::PlaySoundAtLocation(this, GettingHitSound, GetActorLocation());

	check(GetWorld())
	if (!TRIGGER_MANAGER)
	{
		Log::Print("No Trigger Manager found");
		return;
	}
	//Notify all items subscribed to on damage taken in time trigger
	TRIGGER_MANAGER->FillTriggerThreshold(ETriggerTypes::ON_DAMAGE_TAKEN_TIME, ReceivedDamage);
}

void APlayerCharacter::GetAmountOfEnemiesNearby()
{
	check(GetWorld())

	NearbyEnemyCount = 0.f;
	TArray<AActor*> OverlappingActors;

	//Get amount of enemies in EnemyDetectionSphere
	EnemyDetectionSphere->GetOverlappingActors(OverlappingActors);
	for (const auto Actor : OverlappingActors)
	{
		if (Cast<AEnemyCharacter>(Actor))
		{
			NearbyEnemyCount++;
		}
	}

	//Handle trigger
	if (!TRIGGER_MANAGER)
	{
		Log::Print("No Trigger Manager found");
		return;
	}
	if (NearbyEnemyCount <= 0)
		TRIGGER_MANAGER->ActivateTrigger(ETriggerTypes::NO_ENEMIES_NEARBY);
	else
		TRIGGER_MANAGER->DeactivateTrigger(ETriggerTypes::NO_ENEMIES_NEARBY);

	if (const UTrigger* TriggerRef = TRIGGER_MANAGER->FindTrigger(ETriggerTypes::ENEMIES_NEARBY))
		if (NearbyEnemyCount >= TriggerRef->ThreshHold)
			TRIGGER_MANAGER->ActivateTrigger(ETriggerTypes::ENEMIES_NEARBY);
		else
			TRIGGER_MANAGER->DeactivateTrigger(ETriggerTypes::ENEMIES_NEARBY);
}

bool APlayerCharacter::CreateProstheses()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	//Create left prostheses and attach it to player character
	LeftProsthesis = GetWorld()->SpawnActor<AProsthesis>(LeftProsthesisClass,
	                                                     FVector(0, 0, 0),
	                                                     FRotator(0, 0, 0),
	                                                     SpawnParams);

	LeftProsthesis->Init(this);
	LeftProsthesis->AttachProsthesis(GetMesh(), "LeftShoulderSocket");

	//Create right prostheses to player character
	RightProsthesis = GetWorld()->SpawnActor<AProsthesis>(RightProsthesisClass,
	                                                      FVector(0, 0, 0),
	                                                      FRotator(0, 0, 0),
	                                                      SpawnParams);
	RightProsthesis->Init(this);
	RightProsthesis->AttachProsthesis(GetMesh(), "RightShoulderSocket");

	if (!RightProsthesis || !LeftProsthesis)
	{
		Log::Print("Can't Spawn Prostheses");
		return false;
	}

	//Set active prosthesis. Can be changed if default should be the right one
	ActiveProsthesis = LeftProsthesis;
	ActiveProsthesis->IsActive = true;
	InactiveProsthesis = RightProsthesis;

	//Broadcast which will update UI
	ProsthesisSwapUI.Broadcast();
	return true;
}

bool APlayerCharacter::ProsthesisSwap()
{
	//Check if prosthesis swap is possible
	if (!DisableProsthesisSwap)
	{
		//Stop current combat action.
		STATE_MACHINE->DisableCombatState();
		if (!ActiveProsthesis)
		{
			Log::Print("No Active Prosthesis selected in Player");
			return false;
		}
		ActiveProsthesis->IsActive = false;

		//Swap active and inactive prosthesis with each other
		if (ActiveProsthesis == LeftProsthesis)
		{
			ActiveProsthesis = RightProsthesis;
			InactiveProsthesis = LeftProsthesis;
		}
		else
		{
			ActiveProsthesis = LeftProsthesis;
			InactiveProsthesis = RightProsthesis;
		}

		ActiveProsthesis->IsActive = true;

		//Broadcast which will update UI
		ProsthesisSwapUI.Broadcast();
		return true;
	}
	return false;
}

void APlayerCharacter::ActivateDashParticle() const
{
	DashParticle->Activate();
}

void APlayerCharacter::DeactivateDashParticle() const
{
	DashParticle->Deactivate();
}

void APlayerCharacter::ActivateThrusterSound()
{
	if (ThrusterSound)
	{
		ThrusterSoundAudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, ThrusterSound, GetActorLocation());
	}
}

void APlayerCharacter::DeactivateThrusterSound() const
{
	if (!ThrusterSoundAudioComponent)
	{
		Log::Print("No Thruster Sound Audio Component found in Player");
		return;
	}
	ThrusterSoundAudioComponent->SetTriggerParameter("EndThruster");
}

FVector APlayerCharacter::GetPointedHit(FHitResult& Hit, const FName TraceTag, const float RayDistance) const
{
	//Get Player Viewport Rotation and Location
	FVector Location;
	FRotator Rotation;
	if (!GetController())
	{
		Log::Print("No Controller in GetPointedHit()");
		return FVector::Zero();
	}
	GetController()->GetPlayerViewPoint(Location, Rotation);

	// Set up the trace parameters
	const FCollisionQueryParams TraceParams(TraceTag, false, this);

	const FVector End = Location + Rotation.Vector() * RayDistance;

	if (GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECC_Visibility, TraceParams))
	{
		return Hit.Location;
	}

	return End;
}

FVector APlayerCharacter::UseAimAssistForGrappling(FHitResult& Hit, const float RayDistance)
{
	//Get Player Viewport Rotation and Location
	FVector Location;
	FRotator Rotation;
	if (!GetController())
	{
		Log::Print("No Controller found for Player");
		return FVector::Zero();
	}
	GetController()->GetPlayerViewPoint(Location, Rotation);

	//Set radius for sphere trace depending on the current velocity of the player character
	float AimAssistRadius = MaxAimAssistRadius / 100 * GetCharacterMovement()->Velocity.Length() / (
		MaxVelocityForAimAssist /
		100);

	//Clamp radius between min and max radius
	if (AimAssistRadius <= MinAimAssistRadius)
	{
		AimAssistRadius = MinAimAssistRadius;
	}
	else if (AimAssistRadius > MaxAimAssistRadius)
	{
		AimAssistRadius = MaxAimAssistRadius;
	}

	//Set start and end for sphere trace. To make sure the sphere trace is not hitting something behind the player character the start vector is multiplied by radius
	const FVector Start = Location + Rotation.Vector() * (AimAssistRadius * 3);
	const FVector End = Location + Rotation.Vector() * RayDistance;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	const TEnumAsByte TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);

	//Perform sphere trace
	if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, AimAssistRadius,
	                                            TraceChannel, true,
	                                            ActorsToIgnore, ShowAimAssist, Hit, true))
	{
		return Hit.ImpactPoint;
	}

	return End;
}

FVector APlayerCharacter::GetAimVector() const
{
	//Get Player Viewport Rotation and Location
	FVector Location;
	FRotator Rotation;
	GetController()->GetPlayerViewPoint(Location, Rotation);

	return Rotation.Vector();
}

bool APlayerCharacter::CheckIfSwinging() const
{
	if (IsGrappling && GetCharacterMovement()->MovementMode != MOVE_Walking)
		return true;

	return false;
}

void APlayerCharacter::HandleSpringArm(const float DeltaTime) const
{
	//Get player character speed
	const float Speed = GetCharacterMovement()->Velocity.Length();

	//Use speed to determine a new length for the spring arm
	float TargetArmLength = (Speed - MovementSpeed) / 2 + CameraArmLength;

	//Cap spring arm max length
	if (TargetArmLength >= CameraMaxDistance)
		TargetArmLength = CameraMaxDistance;

	//Make sure spring arm doesn't go below default spring arm length
	if (TargetArmLength > CameraArmLength)
	{
		//Interpolate to new arm length
		const float NewArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, TargetArmLength,
		                                            DeltaTime, ZoomInSpeed);

		SpringArm->TargetArmLength = NewArmLength;
	}
	else
	{
		//Interpolate back to normal
		const float NewArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, CameraArmLength, DeltaTime,
		                                            ZoomOutSpeed);

		SpringArm->TargetArmLength = NewArmLength;
	}
}

void APlayerCharacter::UpdateAllStatsForUI()
{
	//The setter will trigger a broadcast to the UI
	SetCurrentHealth(GetCurrentHealth());
	SetCurrencyAmount(GetCurrencyAmount());
	SetPlayerLevel(GetPlayerLevel());
	SetCurrentXP(GetCurrentXP());
	SetCurrentEnergy(GetCurrenEnergy());
	SetAttackDamage(GetAttackDamage());
	SetAttackSpeed(GetAttackSpeed());
	SetCriticalStrikeChance(GetCriticalStrikeChance());
	SetCriticalStrikeMultiplier(GetCriticalStrikeMultiplier());
	SetPullSpeed(GetPullSpeed());
	SetUpgradePoints(GetUpgradePoints());
	SetMovementSpeed(GetMovementSpeed());
	SetCoolDownReduction(GetCoolDownReduction());
	SetHealthRegeneration(GetHealthRegeneration());
}

float APlayerCharacter::DoDamageToCharacter(ABaseCharacter* OtherCharacter, const float DamageScaler,
                                            const float AdditionalCriticalChance, const float AdditionalCriticalMulti)
{
	//Dont do anything if the other character is already dead
	if (OtherCharacter->IsDead) { return -1.f; }

	//Standard damage behavior
	const float DamageDealt = Super::DoDamageToCharacter(OtherCharacter, DamageScaler, AdditionalCriticalChance,
	                                                     AdditionalCriticalMulti);

	//Give XP to player character
	if (Cast<AEnemyCharacter>(OtherCharacter))
	{
		if (OtherCharacter->IsDead)
		{
			if (const AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(OtherCharacter))
			{
				//Add enemy xp drop to player
				SetCurrentXP(GetCurrentXP() + Enemy->XPDrop);

				//Get difference between current and max XP
				const float XPDiff = GetCurrentXP() - GetMaxXP();

				//If diff is larger 0 level up character and set the diff as current XP
				if (XPDiff >= 0.f)
				{
					LevelUP();
					SetCurrentXP(XPDiff);
				}
			}
		}
		else
		{
			if (!GetWorld())
			{
				Log::Print("World not found");
				return -1.f;
			}
			if (!TRIGGER_MANAGER)
			{
				Log::Print("No Trigger Manager found for Player");
				return -1.f;
			}
			//Notify all items subscribed to on damage dealt in time trigger
			TRIGGER_MANAGER->FillTriggerThreshold(ETriggerTypes::ON_DAMAGE_DEALT_TIME, DamageDealt);
		}
	}

	//Broadcast which will update UI
	EnemyHit.Broadcast();
	return DamageDealt;
}

void APlayerCharacter::AddCurrency(float currency)
{
	SetCurrencyAmount(GetCurrencyAmount() + currency);
}

void APlayerCharacter::Interact()
{
	TArray<AActor*> Intractable;

	InteractionSphere->GetOverlappingActors(Intractable);
	for (const auto Actor : Intractable)
	{
		if (AInteractable* InteractableRef = Cast<AInteractable>(Actor); InteractableRef)
		{
			InteractableRef->Interact();
		}
	}
}

void APlayerCharacter::ActivateFreeCam()
{
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void APlayerCharacter::DeactivateFreeCam()
{
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void APlayerCharacter::FinishCurrentCombatState() const
{
	if (!StateMachine->CurrentCombatState)
	{
		Log::Print("No current combat state found for player");
		return;
	}
	StateMachine->CurrentCombatState->End();
}

void APlayerCharacter::NotifyCombatState()
{
	if (!StateMachine->CurrentCombatState)
	{
		Log::Print("No current combat state found for player");
		return;
	}
	StateMachine->CurrentCombatState->AttackNotify();
}

void APlayerCharacter::LevelUP(const float Scalar)
{
	Super::LevelUP(Scalar);

	SetPlayerLevel(GetPlayerLevel() + 1);
	SetMaxXP(GetMaxXP() + AddedXP);
	SetAttackDamage(GetAttackDamage() + AddedAttackDamage);
	SetMaxHealth(GetMaxHealth() + AddedMaxHP);
	SetCurrentHealth(GetMaxHealth());
	SetUpgradePoints(GetUpgradePoints() + 1);

	check(GetWorld())
	//Short delay. This will math the LevelUP text to the particle effect
	GetWorldTimerManager().SetTimer(LevelUpTimer, [this]()
	{
		constexpr FColor Color(245, 114, 39, 255);
		const FLinearColor LinearColor = FLinearColor(Color);
		CombatNumbers->AddCombatNumber(FText::FromString("Level Up"),
		                               CombatNumberLocation, LinearColor);
	}, 0.4f, false);
	ActivateLevelUpParticle();
	if (LevelUpSound)
	{
		UGameplayStatics::PlaySound2D(this, LevelUpSound);
	}
}

void APlayerCharacter::OnCriticalDamage(const float DamageAmount, ABaseCharacter* OtherCharacter)
{
	Super::OnCriticalDamage(DamageAmount, OtherCharacter);

	//Different color for damage number occured through critical damage
	// CombatNumbers->AddCombatNumber(
	// 	FText::AsNumber(FMath::RoundToInt(DamageAmount), &FNumberFormattingOptions::DefaultNoGrouping()),
	// 	OtherCharacter->CombatNumberLocation, FLinearColor(FLinearColor::Red));

	check(GetWorld())
	if (!TRIGGER_MANAGER)
	{
		Log::Print("No Trigger Manager found for Player");
		return;
	}
	//Notify all items subscribed to trigger on critical
	TRIGGER_MANAGER->Notify(OtherCharacter, ETriggerTypes::ON_CRITICAL);
}

void APlayerCharacter::OnNormalDamage(float DamageAmount, ABaseCharacter* OtherCharacter)
{
	Super::OnNormalDamage(DamageAmount, OtherCharacter);
}

void APlayerCharacter::EquipItem(UItem* Item)
{
	if (!Item)
	{
		Log::Print("Can't equip invalid Item on Player");
		return;
	}
	Item->IsEquipped = true;
	Item->EffectActor->OnEquipped();

	InventoryChanged.Broadcast();
}

void APlayerCharacter::UnequipItem(UItem* Item)
{
	if (!Item)
	{
		Log::Print("Can't unequip invalid Item on Player");
		return;
	}
	Item->IsEquipped = false;
	Item->EffectActor->OnUnequipped();

	InventoryChanged.Broadcast();
}

void APlayerCharacter::AddItem(UItem* Item)
{
	if (!Item)
	{
		Log::Print("Can't add invalid Item on Player");
		return;
	}
	ItemInventory.Add(Item);
	InventoryChanged.Broadcast();
}

void APlayerCharacter::RemoveItem(UItem* Item)
{
	if (!Item)
	{
		Log::Print("Can't remove invalid Item on Player");
		return;
	}
	if (!TRIGGER_MANAGER)
	{
		Log::Print("No Trigger Manager found for Player");
		return;
	}
	ItemInventory.Remove(Item);

	//Remove item from subscriber list
	TRIGGER_MANAGER->RemoveItem(Item);
	InventoryChanged.Broadcast();
}

void APlayerCharacter::ScrapItem(UItem* Item)
{
	if (!Item)
	{
		Log::Print("Can't scrap invalid Item on Player");
		return;
	}
	AddCurrency(Item->Effect->ScrapValue);
	AddCurrency(Item->Trigger->ScrapValue);
	RemoveItem(Item);
}

void APlayerCharacter::AddEffectPart(UEffectPart* Effect)
{
	if (!Effect)
	{
		Log::Print("Can't add invalid Effect on Player");
		return;
	}
	EffectPartsInventory.Add(Effect);
	InventoryChanged.Broadcast();
	TriggerNotification.Broadcast("Effect picked up");
}

void APlayerCharacter::RemoveEffectPart(UEffectPart* Effect)
{
	if (!Effect)
	{
		Log::Print("Can't remove invalid Effect on Player");
		return;
	}
	EffectPartsInventory.Remove(Effect);
	InventoryChanged.Broadcast();
}

void APlayerCharacter::ScrapEffectPart(UEffectPart* Effect)
{
	if (!Effect)
	{
		Log::Print("Can't scrap invalid Effect on Player");
		return;
	}
	AddCurrency(Effect->ScrapValue);
	RemoveEffectPart(Effect);
}

void APlayerCharacter::AddTriggerPart(UTriggerPart* Trigger)
{
	if (!Trigger)
	{
		Log::Print("Can't add invalid Trigger on Player");
		return;
	}
	TriggerPartsInventory.Add(Trigger);
	InventoryChanged.Broadcast();
	TriggerNotification.Broadcast("Trigger picked up");
}

void APlayerCharacter::RemoveTriggerPart(UTriggerPart* Trigger)
{
	if (!Trigger)
	{
		Log::Print("Can't remove invalid Trigger on Player");
		return;
	}
	TriggerPartsInventory.Remove(Trigger);
	InventoryChanged.Broadcast();
}

void APlayerCharacter::ScrapTriggerPart(UTriggerPart* Trigger)
{
	if (!Trigger)
	{
		Log::Print("Can't scrap invalid Trigger on Player");
		return;
	}
	AddCurrency(Trigger->ScrapValue);
	RemoveTriggerPart(Trigger);
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	WorldManager::PlayerCharacter = this;
}

void APlayerCharacter::Die()
{
	Super::Die();

	LeftProsthesis->DestroyProsthesis();
	RightProsthesis->DestroyProsthesis();

	bUseControllerRotationYaw = false;
	InputManager->ClearCurrentInputs();

	BuffManager->CleanUp();

	Cast<ADuskbornGameModeBase>(GetWorld()->GetAuthGameMode())->ShutDownWorld();
}

void APlayerCharacter::Jump()
{
	Super::Jump();

	if (!IsPulling)
		IsJumping = true;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	InputComponent->BindAxis("Turn", this, &APlayerCharacter::TurnCamera);
	InputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUp);

	//UI Inputs
	InputComponent->BindAction("Interact / Crafting", IE_Pressed, this, &APlayerCharacter::Interact);

	//Following inputs are handled in the InputManager
	InputComponent->BindAxis("MoveForward", this, &APlayerCharacter::HandleAxisInput<EInputs::FORWARD_BACKWARD>);
	InputComponent->BindAxis("MoveRight", this, &APlayerCharacter::HandleAxisInput<EInputs::RIGHT_LEFT>);

	InputComponent->BindAction("Action", IE_Pressed, this,
	                           &APlayerCharacter::HandleActionInput<EInputs::ACTION, IE_Pressed>);
	InputComponent->BindAction("Action", IE_Released, this,
	                           &APlayerCharacter::HandleActionInput<EInputs::ACTION, IE_Released>);
	InputComponent->BindAction("Secondary Action", IE_Pressed, this,
	                           &APlayerCharacter::HandleActionInput<EInputs::SECONDARY_ACTION, IE_Pressed>);
	InputComponent->BindAction("Secondary Action", IE_Released, this,
	                           &APlayerCharacter::HandleActionInput<EInputs::SECONDARY_ACTION, IE_Released>);
	InputComponent->BindAction("Jump", IE_Pressed, this,
	                           &APlayerCharacter::HandleActionInput<EInputs::JUMP, IE_Pressed>);
	InputComponent->BindAction("Jump", IE_Released, this,
	                           &APlayerCharacter::HandleActionInput<EInputs::JUMP, IE_Released>);
	InputComponent->BindAction("Sprint", IE_Pressed, this,
	                           &APlayerCharacter::HandleActionInput<EInputs::SPRINT, IE_Pressed>);
	InputComponent->BindAction("Sprint", IE_Released, this,
	                           &APlayerCharacter::HandleActionInput<EInputs::SPRINT, IE_Released>);
	InputComponent->BindAction("Dash", IE_Pressed, this,
	                           &APlayerCharacter::HandleActionInput<EInputs::DASH, IE_Pressed>);
	InputComponent->BindAction("Dash", IE_Released, this,
	                           &APlayerCharacter::HandleActionInput<EInputs::DASH, IE_Released>);
	InputComponent->BindAction("Special", IE_Pressed, this,
	                           &APlayerCharacter::HandleActionInput<EInputs::SPECIAL, IE_Pressed>);
	InputComponent->BindAction("Special", IE_Released, this,
	                           &APlayerCharacter::HandleActionInput<EInputs::SPECIAL, IE_Released>);
	InputComponent->BindAction("Ability Swap", IE_Pressed, this,
	                           &APlayerCharacter::HandleActionInput<EInputs::ABILITY_SWAP, IE_Pressed>);
	InputComponent->BindAction("Ability Swap", IE_Released, this,
	                           &APlayerCharacter::HandleActionInput<EInputs::ABILITY_SWAP, IE_Released>);
}
