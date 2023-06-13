// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Weapon.h"
#include "Item.h"
#include "ItemStorage.h"
#include "MainCharacter.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Dead UMETA(DisplayName = "Dead"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),

	ESS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class LIGHTSEEKER_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
	TSubclassOf<AItemStorage> WeaponStorage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bHasCombatTarget;

	FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget; }

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Combat")
	FVector CombatTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class AMainCharPlayerController* MainCharPlayerController;

	//array for item locations
	TArray<FVector> PickupLocations;

	UFUNCTION(BlueprintCallable)
	void ShowPickupLocations();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;

	FORCEINLINE void SetStaminaStatus(EStaminaStatus NewStatus) { StaminaStatus = NewStatus; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintStamina;

	float InterpSpeed;
	bool bInterpToEnemy;
	void SetbInterToEnemy(bool Interp);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AEnemy* CombatTarget;

	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; }

	FRotator GetLookAtRotationYaw(FVector Target); // pass location of combat target as parameter

	// set movement status and running speed
	void SetMovementStatus(EMovementStatus NewStatus);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintingSpeed;

	bool bSprintKeyDown;
	// pressed down for sprinting
	void SprintKeyDown();
	// released to stop sprinting
	void SprintKeyUp();

	/** Cameraboom positioning the camera behind the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	/** Following Camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/* Basic rates to scale turning of the camera*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseLookupRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UParticleSystem* HitParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	USoundCue* HitSound;

	/**
	* 
	* Player Stats
	* 
	*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxHealth;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
	float Health;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
	float Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
	int32 Coins;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void DecreaseHealth(float amount);

	UFUNCTION(BlueprintCallable)
	void IncreaseHealth(float amount);

	void Die(); // call when health < 0 or a death event occurs

	UFUNCTION(BlueprintCallable)
	void IncreaseCoin(int32 amount);

	void DecreaseCoin(int32 amount);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// called for forward/backward input
	void MoveForward(float value);

	bool bMovingForward;

	//called for right/left input
	void MoveRight(float value);

	bool bMovingRight;

	/** Called via input to look up/down at given rate
	* @param rate is a normalised rate i.e 1.0 means 100% of desired look up/down rate
	*/
	void TurnRate(float rate);

	/** Called via input to turn at given rate
	* @param rate is a normalised rate i.e 1.0 means 100% of desired turn rate
	*/
	void LookupRate(float rate);

	virtual void Jump() override;

	bool bLMBDown;
	void LMBDown();
	void LMBUp();

	bool bESCDown;
	void ESCDown();
	void ESCUp();

	FORCEINLINE USpringArmComponent* getCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* getFollowCamera() const { return FollowCamera; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
	AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	AItem* ActiveOverlappingItem;

	void SetEquippedWeapon(AWeapon* Weapon);

	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; }
	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	UAnimMontage* CombatMontage;

	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void PlaySound();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();


	void UpdateCombatTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AEnemy> EnemyFilter;

	void Roll();
	bool bRolling;

	UFUNCTION(BlueprintCallable)
	bool GetRolling();

	UFUNCTION(BlueprintCallable)
	void Invincibility();

	UFUNCTION(BlueprintCallable)
	void InvincibilityEnd();
	
	void SwitchLevel(FName LevelName);

	UFUNCTION(BlueprintCallable)
	void SaveGame();

	UFUNCTION(BlueprintCallable)
	void LoadGame(bool SetPosition);

	void LoadGameNoSwitch();

	UFUNCTION(BlueprintCallable)
	void NewGame();

};
