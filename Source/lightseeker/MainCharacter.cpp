// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h" // for the system lib
#include "Weapon.h"
#include "Kismet/GameplayStatics.h"   // for gameplay statics
#include "Animation/AnimInstance.h"
#include "Kismet/KismetMathLibrary.h" // for find look at rotation
#include "Enemy.h"
#include "MainCharPlayerController.h"
#include "LightseekerSaveGame.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//set size for collision capsule
	GetCapsuleComponent()->SetCapsuleSize(48.f, 105.f);

	// create cameraboom and (pulls towards the player if ther's  a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CamerBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f; //camera follows distance
	CameraBoom->bUsePawnControlRotation = true; //rotate arm based on controller rotation

	//create follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // attach camera to the boom
	FollowCamera->bUsePawnControlRotation = false; // let the boom adjust to match the controller rotation

	//set our turn rates for input
	BaseTurnRate = 65.f;
	BaseLookupRate = 65.f;

	//prevent rotation with controller rotation
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	//face the player towards the direction of input
	GetCharacterMovement()->bOrientRotationToMovement = true; //character moves in the direction of input ...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f); // ... at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 450.f;
	GetCharacterMovement()->AirControl = 0.2f;

	/**
	* 
	* Player Stats
	* 
	*/
	MaxHealth = 100.f;
	MaxStamina = 200.f;
	Health = 100.f;
	Stamina = 200.f;
	Coins = 0;
	// Running/Sprinting
	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;
	bSprintKeyDown = false;

	bLMBDown = false;
	bESCDown = false;

	// Initialise Enums
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	// Stmaina
	StaminaDrainRate = 50.f;
	MinSprintStamina = 50.f;

	InterpSpeed = 15.f;
	bInterpToEnemy = false;

	bHasCombatTarget = false;

	bMovingForward = false;
	bMovingRight = false;

	bRolling = false;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	MainCharPlayerController = Cast<AMainCharPlayerController>(GetController());

	LoadGameNoSwitch();

	if (MainCharPlayerController) {
		FInputModeGameOnly InputMode;
		MainCharPlayerController->SetInputMode(InputMode);
	}
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementStatus == EMovementStatus::EMS_Dead) {
		return; // do nothing and return control to main
	}

	// stamina related operations
	float DeltaStamina = StaminaDrainRate * DeltaTime;

	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if (bSprintKeyDown) {
			if (Stamina - DeltaStamina <= 0.f) {
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina = 0;
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else {
				Stamina = Stamina - DeltaStamina;
				if (bMovingForward || bMovingRight) {
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
				}
				else {
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
			}
		}
		else {
			if (Stamina + DeltaStamina >= MaxStamina) {
				Stamina = MaxStamina;
			}
			else {
				Stamina = Stamina + DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	case EStaminaStatus::ESS_Exhausted:
		if (bSprintKeyDown) {
			Stamina = 0.f;
		}
		else {
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			Stamina = Stamina + DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	case EStaminaStatus::ESS_ExhaustedRecovering:
		if (Stamina + DeltaStamina >= MinSprintStamina) {
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
			Stamina = Stamina + DeltaStamina;
		}
		else {
			Stamina = Stamina + DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	default:
		; // empty statement
	}

	// face the enemy when attacking
	if (bInterpToEnemy && CombatTarget) {
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation()); // destination for character rotation
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed); // for smooth rotation rather than snapping

		SetActorRotation(InterpRotation);
	}

	if (CombatTarget) {
		CombatTargetLocation = CombatTarget->GetActorLocation();
		if (MainCharPlayerController) {
			MainCharPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}

}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainCharacter::SprintKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainCharacter::SprintKeyUp);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainCharacter::Roll);

	PlayerInputComponent->BindAction("PauseTheGame", IE_Pressed, this, &AMainCharacter::ESCDown);
	PlayerInputComponent->BindAction("PauseTheGame", IE_Released, this, &AMainCharacter::ESCUp);

	PlayerInputComponent->BindAction("LMBorRB", IE_Pressed, this, &AMainCharacter::LMBDown);
	PlayerInputComponent->BindAction("LMBorRB", IE_Released, this, &AMainCharacter::LMBUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Lookup", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMainCharacter::TurnRate);
	PlayerInputComponent->BindAxis("LookupRate", this, &AMainCharacter::LookupRate);

}

void AMainCharacter::MoveForward(float value)
{
	bMovingForward = false;

	if ((Controller != nullptr) && (value != 0.0f) && (!bAttacking) && (MovementStatus != EMovementStatus::EMS_Dead)) {
		// find the forward direction
		const FRotator rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, rotation.Yaw,0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, value);

		bMovingForward = true;
	}
}

void AMainCharacter::MoveRight(float value)
{
	bMovingRight = false;

	if ((Controller != nullptr) && (value != 0.0f) && (!bAttacking) && (MovementStatus != EMovementStatus::EMS_Dead)) {
		// find the forward direction
		const FRotator rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, value);

		bMovingRight = true;
	}
}

void AMainCharacter::TurnRate(float rate)
{
	AddControllerYawInput(rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LookupRate(float rate)
{
	AddControllerPitchInput(rate * BaseLookupRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LMBDown()
{
	bLMBDown = true;

	if (MovementStatus == EMovementStatus::EMS_Dead) {
		return; // do nothing and return control to main
	}

	if (ActiveOverlappingItem) {
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon) {
			Weapon->Equip(this);
			SetActiveOverlappingItem(nullptr);
		}
	}
	else if (EquippedWeapon) {
		Attack();
	}
}

void AMainCharacter::LMBUp()
{
	bLMBDown = false;
}

void AMainCharacter::Jump()
{
	if (MovementStatus != EMovementStatus::EMS_Dead) {
		Super::Jump();
	}
}

// stat related
void AMainCharacter::DecreaseHealth(float amount)
{

	if (!bRolling) {
		if (Health - amount <= 0.f) {
			Health = Health - amount;
			Die();
		}
		else {
			Health = Health - amount;
		}
	}

	
}

void AMainCharacter::IncreaseHealth(float amount)
{
	if (Health + amount > 100.f) {
		Health = 100;
	}
	else {
		Health = Health + amount;
	}
}

void AMainCharacter::Die()
{
	if (MovementStatus != EMovementStatus::EMS_Dead) {
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage) {
			AnimInstance->Montage_Play(CombatMontage, 1.0);
			AnimInstance->Montage_JumpToSection(FName("Death"), CombatMontage);
		}
		SetMovementStatus(EMovementStatus::EMS_Dead);
	}
}

void AMainCharacter::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
}

void AMainCharacter::IncreaseCoin(int32 amount) 
{
	Coins = Coins + amount;
}

void AMainCharacter::DecreaseCoin(int32 amount)
{
	if (Coins - amount <= 0.f) {
		UE_LOG(LogTemp, Warning, TEXT("Out of Coins"));
	}
	else {
		Coins = Coins - amount;
	}
}

void AMainCharacter::SetMovementStatus(EMovementStatus NewStatus)
{
	MovementStatus = NewStatus;
	if (MovementStatus == EMovementStatus::EMS_Sprinting) {
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else {
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
}

void AMainCharacter::SprintKeyDown()
{
	bSprintKeyDown = true;
}

void AMainCharacter::SprintKeyUp()
{
	bSprintKeyDown = false;
}

void AMainCharacter::ShowPickupLocations()
{
	for (int32 i = 0; i < PickupLocations.Num(); i++) {
		UKismetSystemLibrary::DrawDebugSphere(this, PickupLocations[i], 25.f, 12, FLinearColor::Black, 10.f, .1f);
	}

	// range based for loop
	/*for (auto Location : PickupLocations) {
		UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 12, FLinearColor::Black, 10.f, .1f);
	}*/
}

void AMainCharacter::SetEquippedWeapon(AWeapon* Weapon)
{
	if (EquippedWeapon) { EquippedWeapon->Destroy(); }
	EquippedWeapon = Weapon;
}

void AMainCharacter::Attack()
{
	if (!bAttacking && !bRolling && (MovementStatus != EMovementStatus::EMS_Dead)) {
		bAttacking = true;
		SetbInterToEnemy(true);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance && CombatMontage) {
			//AnimInstance->Montage_Play(CombatMontage, 2.2f);
			//AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
			//int32 Section = FMath::RandRange(0, 1); // plays a random attack animation when attacking
			
			/*switch (Section) {
			case 0:
				AnimInstance->Montage_Play(CombatMontage, 2.2f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
				Stamina = Stamina - 120;
				break;
			case 1:
				AnimInstance->Montage_Play(CombatMontage, 1.8f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
				Stamina = Stamina - 120;
				break;
			default:
				;
			}*/

			AnimInstance->Montage_Play(CombatMontage, 2.2f);
			AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
			
			if (Stamina - 120 < 0.f) {
				Stamina = -50.f;
			}
			else {
				Stamina = Stamina - 120;
			}

		}
		
	}
	
}

void AMainCharacter::AttackEnd()
{
	bAttacking = false;
	SetbInterToEnemy(false);
}

void AMainCharacter::PlaySound()
{
	if (EquippedWeapon->SwingSound) {
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
	}
}

void AMainCharacter::SetbInterToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

FRotator AMainCharacter::GetLookAtRotationYaw(FVector Target) 
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw = FRotator(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}

float AMainCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (!bRolling) {
		if (Health - DamageAmount <= 0.f) {
			Health = Health - DamageAmount;
			Die();
			if (DamageCauser) {
				AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
				if (Enemy) {
					Enemy->bHasValidAttackTarget = false;
				}
			}
		}
		else {
			Health = Health - DamageAmount;
		}
	}

	return DamageAmount;
}

void AMainCharacter::UpdateCombatTarget()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, EnemyFilter);

	if (OverlappingActors.Num() == 0) { 

		if (MainCharPlayerController) {
			MainCharPlayerController->HideEnemyHealthBar();
		}

		return;
	}

	AEnemy* NearestEnemy = Cast<AEnemy>(OverlappingActors[0]);

	if (NearestEnemy) {
		FVector Location = GetActorLocation();

		float MinimumDistance = (NearestEnemy->GetActorLocation() - Location).Size();

		// range based for loop
		for (auto Actor : OverlappingActors) {

			AEnemy* NME = Cast<AEnemy>(Actor);

			if (NME) {

				float DistToActor = (NME->GetActorLocation() - Location).Size();

				if (DistToActor < MinimumDistance) {
					MinimumDistance = DistToActor;
					NearestEnemy = NME;
				}
			}
		}

		if (MainCharPlayerController) {
			MainCharPlayerController->DisplayEnemyHealthBar();
		}
		SetCombatTarget(NearestEnemy);
		bHasCombatTarget = true;
	}

}

void AMainCharacter::Roll() {
	if (!bAttacking && (MovementStatus != EMovementStatus::EMS_Dead)) {
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance && CombatMontage && Stamina > 50) {

			AnimInstance->Montage_Play(CombatMontage, 2.2f);
			AnimInstance->Montage_JumpToSection(FName("Roll"), CombatMontage);

			if (Stamina - 50 < 0.f) {
				Stamina = -50.f;
			}
			else {
				Stamina = Stamina - 50;
			}
		}
	}
}

void AMainCharacter::Invincibility()
{
	bRolling = true;
}

void AMainCharacter::InvincibilityEnd()
{
	bRolling = false;
}

void AMainCharacter::SwitchLevel(FName LevelName)
{
	UWorld* World = GetWorld();

	if (World) {
		FString CurrentLevel = World->GetMapName();

		FName CurrentLevelName = FName(*CurrentLevel);

		if (CurrentLevelName != LevelName) {
			UGameplayStatics::OpenLevel(World, LevelName);
		}
	}
}

void AMainCharacter::SaveGame()
{
	ULightseekerSaveGame* SaveGameInstance = Cast<ULightseekerSaveGame>(UGameplayStatics::CreateSaveGameObject(ULightseekerSaveGame::StaticClass()));

	SaveGameInstance->CharacterStats.Health = Health;
	SaveGameInstance->CharacterStats.MaxHealth = MaxHealth;
	SaveGameInstance->CharacterStats.Stamina = Stamina;
	SaveGameInstance->CharacterStats.MaxStamina = MaxStamina;
	SaveGameInstance->CharacterStats.Coins = Coins;
	SaveGameInstance->CharacterStats.Location = GetActorLocation();
	SaveGameInstance->CharacterStats.Rotation = GetActorRotation();
	
	FString MapName = GetWorld()->GetMapName();
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	SaveGameInstance->CharacterStats.LevelName = MapName;

	if (EquippedWeapon) {
		SaveGameInstance->CharacterStats.WeaponName = EquippedWeapon->Name;
	}

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerName, SaveGameInstance->PlayerIndex);
}

void AMainCharacter::LoadGame(bool SetPosition)
{
	ULightseekerSaveGame* LoadGameInstance = Cast<ULightseekerSaveGame>(UGameplayStatics::CreateSaveGameObject(ULightseekerSaveGame::StaticClass()));

	if (LoadGameInstance) {
		LoadGameInstance = Cast<ULightseekerSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->PlayerIndex));

		if (LoadGameInstance) {

			Health = LoadGameInstance->CharacterStats.Health;
			MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
			Stamina = LoadGameInstance->CharacterStats.Stamina;
			MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
			Coins = LoadGameInstance->CharacterStats.Coins;

			if (SetPosition) {
				SetActorLocation(LoadGameInstance->CharacterStats.Location);
				SetActorRotation(LoadGameInstance->CharacterStats.Rotation);
			}

			if (WeaponStorage) {

				AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);

				if (Weapons) {

					FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;

					if (WeaponName != "") {
						if (Weapons->WeaponMap.Contains(WeaponName)) {
							AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);

							WeaponToEquip->Equip(this);
						}
					}
				}
			}

			FName LevelName(*LoadGameInstance->CharacterStats.LevelName);
			SwitchLevel(LevelName);

			SetMovementStatus(EMovementStatus::EMS_Normal);
			GetMesh()->bPauseAnims = false;
			GetMesh()->bNoSkeletonUpdate = false;
		}
	}
	
}

void AMainCharacter::ESCDown()
{
	bESCDown = true;

	if (MainCharPlayerController) {
		MainCharPlayerController->TogglePauseMenu();
	}
}

void AMainCharacter::ESCUp()
{
	bESCDown = false;
}

void AMainCharacter::LoadGameNoSwitch()
{
	ULightseekerSaveGame* LoadGameInstance = Cast<ULightseekerSaveGame>(UGameplayStatics::CreateSaveGameObject(ULightseekerSaveGame::StaticClass()));

	if (LoadGameInstance) {
		
		LoadGameInstance = Cast<ULightseekerSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->PlayerIndex));

		if (LoadGameInstance) {

			Health = LoadGameInstance->CharacterStats.Health;
			MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
			Stamina = LoadGameInstance->CharacterStats.Stamina;
			MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
			Coins = LoadGameInstance->CharacterStats.Coins;


			if (WeaponStorage) {

				AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);

				if (Weapons) {

					FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;

					if (WeaponName != "") {
						if (Weapons->WeaponMap.Contains(WeaponName)) {
							AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);

							WeaponToEquip->Equip(this);
						}
					}
				}
			}

			SetMovementStatus(EMovementStatus::EMS_Normal);
			GetMesh()->bPauseAnims = false;
			GetMesh()->bNoSkeletonUpdate = false;
		}
	}
}

void AMainCharacter::NewGame()
{
	SwitchLevel("ElvenRuins");
	Health = 100;
	Coins = 0;
	if (EquippedWeapon) { EquippedWeapon->Destroy(); }
}

bool AMainCharacter::GetRolling() {
	return bRolling;
}