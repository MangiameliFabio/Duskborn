// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyCharacter.h"

#include "Duskborn/DuskbornGameModeBase.h"
#include "Duskborn/Enums.h"
#include "Duskborn/Environment/EnemySpawn.h"
#include "Duskborn/Helper/Helper.h"
#include "Duskborn/Management/WorldManager.h"
#include "Duskborn/UI/CombatNumberComponent.h"
#include "Duskborn/UI/EnemyHealthBar.h"
#include "Duskborn/UpgradeSystem/TriggerManager.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter() : ABaseCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Create the widget component
	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBar->SetupAttachment(RootComponent);

	// Set the widget class for the component (replace UYourWidgetClass with your desired widget class)
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/05_UI/Widgets/WB_EnemyHealth"));
	if (WidgetClassFinder.Succeeded())
	{
		FString WidgetPath = FPaths::ProjectContentDir() + WidgetClassFinder.Class->GetPathName();
		HealthBar->SetWidgetClass(WidgetClassFinder.Class);
	}
}

void AEnemyCharacter::LevelUP(float Scalar)
{
	Super::LevelUP(Scalar);

	const float HealthPercent = GetCurrentHealth() / (GetMaxHealth() / 100) / 100;

	SetMaxHealth(BaseMaxHealth + BaseMaxHealth * (Scalar / 60.f * 0.1));
	SetCurrentHealth(GetMaxHealth() * HealthPercent);
	SetAttackDamage(BaseAttackDamage + BaseAttackDamage * (Scalar / 60.f * 0.1));
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	Player = WorldManager::PlayerCharacter;
	if (!Player)
	{
		Log::Print("Player not found");
	}

	BaseAttackDamage = AttackDamage;
	BaseMaxHealth = MaxHealth;
	CurrentHealth = MaxHealth;
	HealthRegenerationRate = HealthRegeneration;

	if (!GetWorld())
	{
		Log::Print("World not found");
		return;
	}

	ADuskbornGameModeBase* GameModeRef = Cast<ADuskbornGameModeBase>(GetWorld()->GetAuthGameMode());
	check(GameModeRef);

	GameModeRef->EnemiesInLevel.Add(this);

	if (GameModeRef->EnemiesByClassInLevel.Contains(EnemyType))
	{
		// Increment the spawn count for this enemy class.
		int& SpawnCount = GameModeRef->EnemiesByClassInLevel[EnemyType];
		SpawnCount++;
	}
	else
	{
		// If the enemy class is not in the map, add it with a spawn count of 1.
		GameModeRef->EnemiesByClassInLevel.Add(EnemyType, 1);
	}

	HealthBarRef = Cast<UEnemyHealthBar>(HealthBar->GetUserWidgetObject());
	HealthBar->SetCollisionProfileName("NoCollision");

	LevelUP(WORLD_TIME);

	DisableHealthBar();

	//Create loop which checks periodically if the enemy should be deleted
	if (DeleteWhenToFarAway)
	{
		GetWorldTimerManager().SetTimer(DeleteTimer, FTimerDelegate::CreateWeakLambda(
			                                this, [this]()
			                                {
				                                //Delete instantly if absolute distance is reached
				                                if (AbsoluteDistanceToPlayer < FVector::Dist(
					                                PLAYER->GetActorLocation(), GetActorLocation()))
				                                {
					                                CleanUp();
					                                Destroy();
				                                }
				                                //If only the max distance is reached also check if the enemy is in FOV
				                                else if (MaxDistanceToPlayer < FVector::Dist(
						                                PLAYER->GetActorLocation(), GetActorLocation())

					                                && !Helper::CheckIfLocationIsInFOV(
						                                GetWorld(), GetActorLocation()))
				                                {
					                                CleanUp();
					                                Destroy();
				                                }
			                                }), DeleteCycle, true);
	}
}

void AEnemyCharacter::UpdateHealthUI() const
{
	HealthBarRef->CurrentHealthPercent = GetCurrentHealth() / (GetMaxHealth() / 100) / 100;

	FRotator Rotation(FRotator::ZeroRotator);
	FVector Location(FVector::Zero());

	PLAYER->GetController()->GetPlayerViewPoint(Location, Rotation);

	const FRotator HealthBarRotation = (Location - PLAYER->GetActorLocation()).Rotation();
	HealthBar->SetWorldRotation(HealthBarRotation);
}

void AEnemyCharacter::EnableHealthBar()
{
	HealthBarVisible = true;
	HealthBar->SetTickMode(ETickMode::Enabled);
	HealthBar->SetVisibility(true);

	if (!GetWorld())
	{
		Log::Print("World not found");
		return;
	}
	GetWorldTimerManager().SetTimer(HealthBarTimer, this, &AEnemyCharacter::DisableHealthBar, HealthBarDuration);
}

void AEnemyCharacter::DisableHealthBar()
{
	HealthBarVisible = false;
	HealthBar->SetTickMode(ETickMode::Enabled);
	HealthBar->SetVisibility(false);
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HealthBarVisible && !RemoveHealthBar)
		UpdateHealthUI();

	if (Player)
	{
		DistanceToPlayer = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
	}
}

void AEnemyCharacter::ReceiveDamage(float ReceivedDamage, bool Critical)
{
	if (Critical)
	{
		//Spawn damage numbers with color for critical hit
		CombatNumbers->AddCombatNumber(
			FText::AsNumber(FMath::RoundToInt(ReceivedDamage), &FNumberFormattingOptions::DefaultNoGrouping()),
			CombatNumberLocation, FLinearColor::Red);
	}
	else
	{
		//Spawn damage numbers with base color
		CombatNumbers->AddCombatNumber(
			FText::AsNumber(FMath::RoundToInt(ReceivedDamage), &FNumberFormattingOptions::DefaultNoGrouping()),
			CombatNumberLocation);
	}

	GotHit = true;

	if (!GetWorld())
	{
		Log::Print("World not found");
		return;
	}
	GetWorldTimerManager().SetTimer(ReceivedDamageTimer, [this]()
	{
		GotHit = false;
	}, CooldownAfterReceivedDamage, false);


	if (!RemoveHealthBar)
		EnableHealthBar();

	Super::ReceiveDamage(ReceivedDamage);
	UGameplayStatics::PlaySoundAtLocation(this, EnemyHitSound, GetActorLocation());
}

void AEnemyCharacter::Die()
{
	Super::Die();
	if (!GetWorld())
	{
		Log::Print("World not found");
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(this, EnemyDeathSound, GetActorLocation());
	TRIGGER_MANAGER->Notify(this, ETriggerTypes::ON_KILL);

	if (!GetWorld())
	{
		Log::Print("World not found");
		return;
	}
	PLAYER->AddCurrency(CurrencyDrop);

	if (!RemoveHealthBar)
		DisableHealthBar();
}

void AEnemyCharacter::CleanUp()
{
	Super::CleanUp();

	ADuskbornGameModeBase* GameModeRef = Cast<ADuskbornGameModeBase>(GetWorld()->GetAuthGameMode());
	check(GameModeRef);

	GameModeRef->EnemiesInLevel.Remove(this);
	if (GameModeRef->EnemiesByClassInLevel.Contains(EnemyType))
	{
		// Increment the spawn count for this enemy class.
		int& SpawnCount = GameModeRef->EnemiesByClassInLevel[EnemyType];
		SpawnCount--;
	}
}

void AEnemyCharacter::Init(AEnemySpawn* Spawner)
{
	EnemySpawn = Spawner;
}

bool AEnemyCharacter::IsPlayerInAttackRange()
{
	if (this->IsPlayerInTriggerDistance(TriggerDistanceAttackState))
	{
		return this->IsPlayerInSight();
	}
	return false;
}

bool AEnemyCharacter::IsPlayerInTriggerDistance(float TriggerDistance)
{
	if (Player == nullptr)
	{
		return false;
	}

	if (DistanceToPlayer < TriggerDistance)
	{
		return true;
	}
	return false;
}

bool AEnemyCharacter::IsPlayerInSight()
{
	if (Player == nullptr)
	{
		return false;
	}
	FHitResult HitResult = FHitResult();
	FVector Start = GetActorLocation();
	FVector End = Player->GetActorLocation();
	FCollisionQueryParams Col = FCollisionQueryParams();
	Col.AddIgnoredActor(this);

	if (!GetWorld())
	{
		Log::Print("World not found");
		return false;
	}
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel2, Col))
	{
		if (Cast<APlayerCharacter>(HitResult.GetActor()))
		{
			return true;
		}
		return false;
	}
	return false;
}

bool AEnemyCharacter::IsEnemyFullyVisibleToPlayer()
{
	if (Player == nullptr)
	{
		return false;
	}
	if (!GetWorld())
	{
		Log::Print("World not found");
		return false;
	}
	FVector Origin;
	FVector BoxExtent;
	GetActorBounds(false, Origin, BoxExtent);
	TArray<FVector> StartPoints;
	StartPoints.Add(GetActorLocation() + FVector(BoxExtent.X / 2, GetActorLocation().Y, GetActorLocation().Z));
	StartPoints.Add(GetActorLocation() - FVector(BoxExtent.X / 2, GetActorLocation().Y, GetActorLocation().Z));
	for (auto StartPoint : StartPoints)
	{
		FHitResult HitResult = FHitResult();
		FVector Start = StartPoint;
		FVector End = Player->GetActorLocation();
		FCollisionQueryParams Col = FCollisionQueryParams();
		Col.AddIgnoredActor(this);
		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel2, Col))
		{
			if (!Cast<APlayerCharacter>(HitResult.GetActor()))
			{
				return false;
			}
		}
	}
	return true;
}

// Shoots bullet in direction of location where the player will be according to his current movement
void AEnemyCharacter::ShootProjectileAtPlayer()
{
	if (Player)
	{
		const FActorSpawnParameters SpawnParams;
		const FVector PlayerLocation = Player->GetActorLocation();
		const FVector PlayerVelocity = Player->GetVelocity();

		// Calculate the estimated time of flight for the bullet to reach the player
		const float TimeToReachPlayer = (PlayerLocation - GetActorLocation()).Size() / BulletSpeed;

		// Predict the player's future position based on their current position and velocity
		const FVector PredictedPlayerPosition = PlayerLocation + PlayerVelocity * TimeToReachPlayer;

		// Use the predicted player position in your calculations
		const FVector VectorBetweenEnemyAndTarget = PredictedPlayerPosition - GetActorLocation();

		const FRotator TargetRotation = VectorBetweenEnemyAndTarget.Rotation();
		const FTransform SpawnTransform(TargetRotation, GetActorLocation());
		ABullet* Bullet = Cast<ABullet>(
			UGameplayStatics::BeginDeferredActorSpawnFromClass(this, BulletClass, SpawnTransform));
		if (Bullet != nullptr)
		{
			Bullet->FiredByCharacter = this;
			Bullet->ProjectileMovementComponent->MaxSpeed = BulletSpeed;
			Bullet->ProjectileMovementComponent->InitialSpeed = BulletSpeed;
			UGameplayStatics::FinishSpawningActor(Bullet, SpawnTransform);
			BulletSpawned();
		}
	}
}

void AEnemyCharacter::ShootProjectileAtPlayerFromLocation(FVector Location)
{
	if (Player != nullptr)
	{
		const FActorSpawnParameters SpawnParams;
		const FVector VectorBetweenDroneAndPlayer = Player->GetActorLocation() - Location;
		const FRotator TargetRotation = VectorBetweenDroneAndPlayer.Rotation();
		FTransform SpawnTransform(TargetRotation, Location);
		ABullet* Bullet = Cast<ABullet>(
			UGameplayStatics::BeginDeferredActorSpawnFromClass(this, BulletClass, SpawnTransform));
		if (Bullet != nullptr)
		{
			Bullet->FiredByCharacter = this;
			UGameplayStatics::FinishSpawningActor(Bullet, SpawnTransform);
			BulletSpawned();
		}
	}
}

FRotator AEnemyCharacter::GetRotationTowardsPlayer()
{
	if (Player)
	{
		FVector Vector = Player->GetActorLocation() - GetActorLocation();
		Vector.Normalize();
		return Vector.Rotation();
	}
	return FRotator(0, 0, 0);
}
