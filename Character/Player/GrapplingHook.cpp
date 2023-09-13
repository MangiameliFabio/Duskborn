// Fill out your copyright notice in the Description page of Project Settings.


#include "Duskborn/Character/Player/GrapplingHook.h"

#include "Duskborn/Management/WorldManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicsEngine/PhysicsSettings.h"

// Sets default values
AGrapplingHook::AGrapplingHook() : AActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Hook = CreateDefaultSubobject<UStaticMeshComponent>("Hook");
	SetRootComponent(Hook);

	PlayerAttachmentSphere = CreateDefaultSubobject<USphereComponent>("PlayerAttachmentPoint");
	PlayerAttachmentSphere->SetupAttachment(GetRootComponent());

	PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>("PhysicsConstraint");
	PhysicsConstraint->SetupAttachment(GetRootComponent());
}

void AGrapplingHook::SetupGrapplingHook(const FVector& GrapplingLocation, const FVector& PlayerLocation,
                                        const FVector& StartVelocity, UPrimitiveComponent* NewGrappledComponent)
{
	//Check if grappled point is part of an enmey
	if (NewGrappledComponent)
	{
		//Use component of enemy to attach hook to it
		GrappledComponent = NewGrappledComponent;
		Hook->AttachToComponent(GrappledComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
	else
	{
		//Set hook world location to grappled point
		Hook->SetWorldLocation(GrapplingLocation, false, nullptr, ETeleportType::ResetPhysics);
	}

	//Set attachment sphere location to current player character location
	PlayerAttachmentSphere->SetWorldLocation(PlayerLocation, false, nullptr, ETeleportType::ResetPhysics);

	//Setup PhysicsConstraint. Set constraint components to PlayerAttachmentSphere and Hook
	PhysicsConstraint->SetWorldLocation(GrapplingLocation);
	PhysicsConstraint->SetConstrainedComponents(PlayerAttachmentSphere, "None", Hook, "None");

	//Physics need a reset to make the hook work properly
	PlayerAttachmentSphere->SetSimulatePhysics(false);
	PlayerAttachmentSphere->SetSimulatePhysics(true);
	PlayerAttachmentSphere->SetAllPhysicsLinearVelocity(StartVelocity);
}

void AGrapplingHook::DetachGrapplingHook() const
{
	Hook->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void AGrapplingHook::AddSwingForce(const FVector& SwingForce) const
{
	PlayerAttachmentSphere->AddForce(SwingForce);
}

bool AGrapplingHook::CheckDownForce()
{
	const FVector Gravity = GetGravityLength();
	const float Mass = PlayerAttachmentSphere->GetMass();

	const float DownForceStrength = (Gravity * Mass).Length();
	const float RadialForceStrength = (Mass * (PlayerAttachmentSphere->GetComponentVelocity() * PlayerAttachmentSphere
		->
		GetComponentVelocity()) / FVector::Dist(PlayerAttachmentSphere->GetRelativeLocation(),
		                                        Hook->GetRelativeLocation())).Length();

	if (PlayerAttachmentSphere->GetRelativeLocation().Z - 88.f > Hook->GetRelativeLocation().Z && DownForceStrength >
		RadialForceStrength)
	{
		return true;
	}
	return false;
}

void AGrapplingHook::BeginPlay()
{
	Super::BeginPlay();

	if (WorldManager::PlayerCharacter)
	{
		PlayerAttachmentSphere->SetSphereRadius(
			WorldManager::PlayerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius() + 10.f);
	}
}

FVector AGrapplingHook::GetGravityLength()
{
	if (const UPhysicsSettings* PhysicsSettings = UPhysicsSettings::Get())
	{
		return PhysicsSettings->DefaultGravityZ * FVector::UpVector;
	}

	// Return default gravity direction if we couldn't get the current one
	return FVector(0.0f, 0.0f, -1.0f);
}
