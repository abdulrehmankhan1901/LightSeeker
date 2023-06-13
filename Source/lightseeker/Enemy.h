// Fill out your copyright notice in the Description page of Project Settings.


// include AIModule in build.cs/PublicDependencies section, similar to when adding dependencies when adding HUD

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "MainCharacter.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyMovementStatus :uint8
{
	EMS_Idle UMETA(DisplayName = "Idle"),
	EMS_MoveToTarget UMETA(DisplayName = "MoveToTarget"),
	EMS_Attacking UMETA(DisplayName = "Attacking"),
	EMS_Dead UMETA(DisplayName = "Dead"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class LIGHTSEEKER_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	bool bHasValidAttackTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float InterpSpeed;

	bool bInterpToPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AnimPlayRate;

	void SetbInterToPlayer(bool Interp);

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Combat")
	FVector CombatTargetLocation;

	FRotator GetLookAtRotationYaw(FVector Target); // pass location of combat target as parameter

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	EEnemyMovementStatus EnemyMovementStatus;

	FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus NewStatus) { EnemyMovementStatus = NewStatus; }
	FORCEINLINE EEnemyMovementStatus GetEnemyMovementStatus() { return EnemyMovementStatus; }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	USphereComponent* AgroSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	USphereComponent* CombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	AAIController* AIController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Health;

	UFUNCTION(BlueprintCallable)
	float GetCurrentHealth();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	USoundCue* SwingSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	USoundCue* HitSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	class UBoxComponent* CombatCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* CombatMontage;


	FTimerHandle AttackTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackMinTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackMaxTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	FTimerHandle DeathTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DeathDelay;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()	
	virtual void AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()	
	virtual void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void MoveToTarget(AMainCharacter* Target);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool bOverlappingCombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	AMainCharacter* CombatTarget;

	UFUNCTION()
	void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void ActivateCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bAttacking;

	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void Die(AActor* Causer);

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	bool Alive();

	void Disappear();
};
