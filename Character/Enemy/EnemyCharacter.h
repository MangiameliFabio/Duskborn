#pragma once

#include "CoreMinimal.h"
#include "Duskborn/Character/BaseClasses/BaseCharacter.h"
#include "StateMachine/EnemyStateMachine.h"
#include "Duskborn/Bullet/Bullet.h"
#include "Components/WidgetComponent.h"
#include "EnemyCharacter.generated.h"

enum class EEnemies : uint8;
class AEnemySpawn;
class UEnemyHealthBar;
class UStateMachine;

// Base class for an enemy character, that all enemies should inherit from
UCLASS()
class DUSKBORN_API AEnemyCharacter : public ABaseCharacter
{
	GENERATED_BODY()

	//-----------------------Variables----------------------------------------------------------------------------------  

	//public variables
public:
	UPROPERTY()
	UEnemyStateMachine* StateMachine = nullptr;

	UPROPERTY(EditAnywhere, Category="Duskborn|Stats")
	float CurrencyDrop = 10.f;
	UPROPERTY(EditAnywhere, Category="Duskborn|Stats")
	float XPDrop = 100.f;
	UPROPERTY(EditAnywhere, Category="Duskborn|State|Attack")
	float TriggerDistanceAttackState = 1000;
	UPROPERTY(BlueprintReadOnly, Category="Duskborn|Player")
	APlayerCharacter* Player = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Duskborn|Bullet")
	TSubclassOf<ABullet> BulletClass = nullptr;
	UPROPERTY(EditAnywhere, Category = "Duskborn|Bullet")
	float BulletSpeed = 8000.0f;
	UPROPERTY(EditAnywhere, Category = "Duskborn|Sound")
	USoundBase* EnemyHitSound = nullptr;
	UPROPERTY(EditAnywhere, Category = "Duskborn|Sound")
	USoundBase* EnemyDeathSound = nullptr;
	UPROPERTY(EditAnywhere, Category = "Duskborn|Sound")
	USoundBase* EnemyFiringBulletSound = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* HealthBar = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UEnemyHealthBar* HealthBarRef = nullptr;
	float DistanceToPlayer = 999999999.0f;
	
	UPROPERTY(EditAnywhere, Category="Duskborn", BlueprintReadOnly)
	float CooldownAfterReceivedDamage = 10.f;

	/** Max distance to player. If distance is higher the enemy can be deleted */
	UPROPERTY(EditAnywhere, Category="Duskborn")
	float MaxDistanceToPlayer = 65000;

	/** If distance to player is higher the enemy is directly deleted*/
	UPROPERTY(EditAnywhere, Category="Duskborn")
	float AbsoluteDistanceToPlayer = 80000;
	
	UPROPERTY(EditAnywhere, Category="Duskborn")
	EEnemies EnemyType;
	
	UPROPERTY(EditDefaultsOnly, Category = "Duskborn")
	bool RemoveHealthBar = false;
	/** If this value is true the enemy character will despawn if its to far away from the player character*/
	UPROPERTY(EditDefaultsOnly, Category = "Duskborn")
	bool DeleteWhenToFarAway = false;
	
	bool GotHit;

	

	//protected variables
protected:
	//private variables
private:
	UPROPERTY()
	AEnemySpawn* EnemySpawn = nullptr;

	FTimerHandle HealthBarTimer;
	FTimerHandle ReceivedDamageTimer;
	FTimerHandle DeleteTimer;

	float DeleteCycle = 5.f;
	float HealthBarDuration = 5.f;

	bool HealthBarVisible = false;
	//-----------------------Functions----------------------------------------------------------------------------------

	//public functions  
public: // Sets default values for this character's properties
	AEnemyCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void ReceiveDamage(float ReceivedDamage, bool Critical = false) override;
	virtual void Die() override;
	virtual void CleanUp() override;
	virtual void Init(AEnemySpawn* Spawner);

	bool IsPlayerInAttackRange();
	bool IsPlayerInTriggerDistance(float TriggerDistance);
	UFUNCTION(BlueprintCallable)
	bool IsPlayerInSight();
	UFUNCTION(BlueprintCallable)
	bool IsEnemyFullyVisibleToPlayer();
	UFUNCTION(BlueprintCallable)
	FRotator GetRotationTowardsPlayer();

	void ShootProjectileAtPlayer();
	void ShootProjectileAtPlayerFromLocation(FVector Location);


	// Blueprint Events
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnExplosion();
	UFUNCTION(BlueprintImplementableEvent)
	void BulletSpawned();

	virtual void LevelUP(float Scalar) override;

	//protected functions
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//private functions  
private:
	void UpdateHealthUI() const;
	void EnableHealthBar();
	void DisableHealthBar();
};
