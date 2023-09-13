// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructorEnemy.h"

#include "DestructorDamage.h"
#include "NiagaraComponent.h"
#include "Duskborn/Management/WorldManager.h"
#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "StateMachine/DestructorStates.h"
#include "StateMachine\DestructorStateMachine.h"


// Sets default values
ADestructorEnemy::ADestructorEnemy() : AEnemyCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StateMachine = CreateDefaultSubobject<UDestructorStateMachine>("Destructor Machine");

	Platform = CreateDefaultSubobject<UStaticMeshComponent>("Platform");
	Platform->SetupAttachment(RootComponent);

	Turntable = CreateDefaultSubobject<UStaticMeshComponent>("Turntable");
	Turntable->SetupAttachment(Platform);

	Cannon = CreateDefaultSubobject<UStaticMeshComponent>("Cannon");
	Cannon->SetupAttachment(Turntable);

	Muzzle = CreateDefaultSubobject<USceneComponent>("Muzzle");
	Muzzle->SetupAttachment(Cannon);
}

// Called when the game starts or when spawned
void ADestructorEnemy::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->DisableMovement();
}

// Called every frame
void ADestructorEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


bool ADestructorEnemy::CheckIfPlayerInRange() const
{
	if (FVector::Dist(PLAYER->GetActorLocation(), GetActorLocation()) <= DetectionRange)
	{
		return true;
	}

	return false;
}

void ADestructorEnemy::RotateToPlayer(const float DeltaTime) const
{
	check(Turntable);

	//Get relative rotation of the turntable
	const FRotator TurntableRotation = Turntable->GetRelativeRotation();

	// Calculate the turntable aim rotation in world space towards the player's location
	FRotator TurntableAimRotation = (PLAYER->GetActorLocation() - GetActorLocation()).Rotation();

	// Convert the turntable aim rotation into a relative rotation for the destructor
	WorldToLocalRotation(TurntableAimRotation);

	// Only change the yaw to rotate the turntable. Use the converted aim rotation.
	const FRotator NewTurntableRotation(TurntableRotation.Pitch, TurntableAimRotation.Yaw,
	                                    TurntableRotation.Roll);

	// Interpolate the turntable to the new rotation
	Turntable->SetRelativeRotation(FMath::RInterpTo(TurntableRotation,
	                                                NewTurntableRotation, DeltaTime, TurntableSpeed));

	check(Cannon);

	// Get the relative rotation of the cannon
	const FRotator CannonRotation = Cannon->GetRelativeRotation();

	// Calculate the cannon aim rotation in world space towards the player's location.
	// Use the Muzzle component for vector calculation.
	FRotator CannonAimRotation = (PLAYER->GetActorLocation() - Muzzle->GetComponentLocation()).Rotation();

	// Convert the cannon aim rotation into a relative rotation for the destructor
	WorldToLocalRotation(CannonAimRotation);

	// Use the aim rotation to set the pitch of the cannon rotation, clamping the value between the min and max angles.
	const FRotator NewCannonRotation(FMath::Clamp(CannonAimRotation.Pitch, CannonMinAngle, CannonMaxAngle),
	                                 CannonRotation.Yaw,
	                                 CannonRotation.Roll);

	// Interpolate the cannon to the new rotation
	Cannon->SetRelativeRotation(FMath::RInterpTo(CannonRotation,
	                                             NewCannonRotation, DeltaTime, CannonSpeed));
}

void ADestructorEnemy::ChangeLaserSize()
{
	// Calculate the distance between the Muzzle and the player's location,
	// and add an offset to ensure the laser shoots through the player
	const float MuzzleDistToPlayer = FVector::Dist(Muzzle->GetComponentLocation(),PLAYER->GetActorLocation()) +
		2000.f;

	if (DestructorDamage)
	{
		// Change DestructorDamage collision size based on the calculated distance
		DestructorDamage->ChangeCollisionSize(MuzzleDistToPlayer);
	}

	// Change the size of the Niagara effect (assuming there is a function named ChangeNiagaraSize)
	ChangeNiagaraSize(MuzzleDistToPlayer);
}

void ADestructorEnemy::GenerateHits() const
{
	if (DestructorDamage)
	{
		check(DestructorDamage->DamageMesh);

		// Generate hit events in the DamageObject
		DestructorDamage->GenerateHitEvents(DestructorDamage->DamageMesh);

		// Empty the HitCharacters array to enable the player to be damaged again
		DestructorDamage->ClearHitCharacters();
	}
}

void ADestructorEnemy::SpawnDamageObject()
{
	if (!GetWorld())
	{
		Log::Print("No World ref in Destructor spawn damage object");
		return;
	}

	//Spawn Damage Object
	DestructorDamage = GetWorld()->SpawnActor<ADestructorDamage>(DestructorDamageClass, Muzzle->GetComponentLocation(),
	                                                             Muzzle->GetComponentRotation());
	if (DestructorDamage)
	{
		//Initialize DamageObject, Attach to Destructor muzzle location and set width of th laser beam
		DestructorDamage->Init(1, 0, 0, this);
		DestructorDamage->AttachToComponent(Muzzle, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		DestructorDamage->DamageMesh->SetRelativeScale3D(FVector(BeamWidth / 100, BeamWidth / 100, 1.f));
	}
	else
	{
		Log::Print("Destructor Damage not spawned. Set a Damage class in Blueprint");
	}
}

void ADestructorEnemy::DeleteDamageObject()
{
	if (!DestructorDamage) { return; }
	DestructorDamage->Destroy();
	DestructorDamage = nullptr;
}

void ADestructorEnemy::StartCharging()
{
	if (IsDead) return;

	ActivateNiagara();
	ChargingSoundAudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, ChargingSound, GetActorLocation());
	GetWorldTimerManager().SetTimer(ChargingTimer, this, &ADestructorEnemy::StartLaser, ChargeDuration);
	Charging = true;
}

void ADestructorEnemy::StartLaser()
{
	Charging = false;
	FiringLaser = true;
	SpawnDamageObject();
	if (ChargingSoundAudioComponent)
	{
		ChargingSoundAudioComponent->Deactivate();
	}
	FiringSoundAudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, FiringSound, GetActorLocation());
	GetWorldTimerManager().SetTimer(LaserTimer, this, &ADestructorEnemy::StartReload, LaserDuration);
	GetWorldTimerManager().SetTimer(ReceivedDamageTimer, this, &ADestructorEnemy::GenerateHits, 1 * GetAttackSpeedAsMultiplier(), true);
}

void ADestructorEnemy::StartReload()
{
	FiringLaser = false;
	DeleteDamageObject();
	DeactivateNiagara();
	if (FiringSoundAudioComponent)
	{
		FiringSoundAudioComponent->Deactivate();
	}
	if (!GetWorld())
	{
		Log::Print("World not found");
		return;
	}
	GetWorldTimerManager().SetTimer(ChargingTimer, [this]()
	{
		Reloaded = true;
	}, ReloadDuration, false);
	GetWorldTimerManager().ClearTimer(ReceivedDamageTimer);
}

void ADestructorEnemy::Die()
{
	Super::Die();

	DeleteDamageObject();
	if (FiringSoundAudioComponent)
	{
		FiringSoundAudioComponent->Deactivate();
	}

	// Set Cannon to simulate physics. This will create the death effect for destructor.
	Cannon->SetSimulatePhysics(true);
	Cannon->SetCollisionProfileName("PhysicsObject");

	//Small Impulse to push cannon out of the mounting.
	Cannon->AddImpulseAtLocation(Cannon->GetForwardVector() * 25000000.f + Cannon->GetComponentLocation(),
	                             Cannon->GetComponentLocation());
	DeactivateNiagara();
}

void ADestructorEnemy::WorldToLocalRotation(FRotator& WorldRotation) const
{
	const FTransform DestructorTransform = GetTransform();
	const FQuat DestructorRotation = DestructorTransform.GetRotation();
	const FQuat WorldRotationQuat = FQuat(WorldRotation);
	const FQuat LocalRotationQuat = DestructorRotation.Inverse() * WorldRotationQuat;
	WorldRotation = LocalRotationQuat.Rotator();
}
