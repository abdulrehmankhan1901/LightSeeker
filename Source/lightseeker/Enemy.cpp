// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "MainCharPlayerController.h"
#include "Kismet/KismetMathLibrary.h" // for find look at rotation



// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(600.f);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.f);

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetMesh(), FName("EnemySocket"));
	//CombatCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("EnemySocket"));

	bOverlappingCombatSphere = false;

	Health = 75.f;
	MaxHealth = 100.f;
	Damage = 10.f;

	bAttacking = false;

	AttackMinTime = 1.0f;
	AttackMaxTime = 4.0f;

	EnemyMovementStatus = EEnemyMovementStatus::EMS_Idle;

	DeathDelay = 5.f;

	bHasValidAttackTarget = false;

	InterpSpeed = 2.f;
	bInterpToPlayer = false;

	AnimPlayRate = 1.f;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());
	
	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);

	// setup collision for only when overlapping with player
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterpToPlayer && CombatTarget) {
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation()); // destination for character rotation
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed); // for smooth rotation rather than snapping

		SetActorRotation(InterpRotation);
	}

	/*if (CombatTarget) {
		CombatTargetLocation = CombatTarget->GetActorLocation();
		if (MainCharPlayerController) {
			MainCharPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}*/

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && Alive()) {
		AMainCharacter* MC = Cast<AMainCharacter>(OtherActor);
		if (MC) {
			MoveToTarget(MC);
		}
	}
}

void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor) {
		AMainCharacter* MC = Cast<AMainCharacter>(OtherActor);

		if (MC) {

			bHasValidAttackTarget = false;

			if (MC->CombatTarget) {
				MC->SetCombatTarget(nullptr);
			}

			MC->SetHasCombatTarget(false);
			MC->UpdateCombatTarget();

			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);

			if (AIController) {
				AIController->StopMovement();
			}
		}
	}
}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && Alive()) {
		AMainCharacter* MC = Cast<AMainCharacter>(OtherActor);
		if (MC) {

			bHasValidAttackTarget = true;

			MC->SetCombatTarget(this);
			MC->SetHasCombatTarget(true);

			MC->UpdateCombatTarget();

			CombatTarget = MC;
			bOverlappingCombatSphere = true;

			GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, 1);
			
			float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
			GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
		}
	}
}

void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherComp) {
		AMainCharacter* MC = Cast<AMainCharacter>(OtherActor);
		if (MC) {
			bOverlappingCombatSphere = false;

			//if (EnemyMovementStatus == EEnemyMovementStatus::EMS_Attacking) {
			//	MoveToTarget(MC);
			//	CombatTarget = nullptr;
			//}
			MoveToTarget(MC);
			CombatTarget = nullptr;

			if (MC->CombatTarget == this) {
				MC->SetCombatTarget(nullptr);
				MC->bHasCombatTarget = false;
				MC->UpdateCombatTarget();
			}

			if (MC->MainCharPlayerController) {
				USkeletalMeshComponent* MCMesh = Cast<USkeletalMeshComponent>(OtherComp);
				if (MCMesh) {
					MC->MainCharPlayerController->HideEnemyHealthBar();
				}
			}

			GetWorldTimerManager().ClearTimer(AttackTimer);
		}
	}
}

void AEnemy::MoveToTarget(AMainCharacter* Target)
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);

	if (AIController) {
		// refer to the AAIController::MoveTo documentation for implementation that cannot be summarised in the comments

		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(10.0f);

		FNavPathSharedPtr NavPath;

		AIController->MoveTo(MoveRequest, &NavPath);

		// debug sphere generation, causes the engine to crash when called multiple times before the initial generation has ended, hence, this is commented out
		/*auto PathPoints = NavPath->GetPathPoints();
		for (auto Point : PathPoints) {
			FVector Location = Point.Location;

			UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 12, FLinearColor::Black, 10.f, 1.f);
		}*/
	}
}

void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor) {
		AMainCharacter* MC = Cast<AMainCharacter>(OtherActor);
		if (MC) {
			if (MC->HitParticles && MC->bRolling == false) {
				const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName("TipSocket");
				if (TipSocket) {
					FVector SocketLocation = TipSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MC->HitParticles, SocketLocation, FRotator(0.f), false);
				}
			}
			if (MC->HitSound && MC->bRolling == false) {
				UGameplayStatics::PlaySound2D(this, MC->HitSound);
			}
			if (DamageTypeClass) {
				UGameplayStatics::ApplyDamage(MC, Damage, AIController, this, DamageTypeClass);
			}
		}
	}
}

void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AEnemy::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	if (SwingSound) {
		UGameplayStatics::PlaySound2D(this, SwingSound);
	}

}

void AEnemy::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::Attack() 
{
	if (Alive() && bHasValidAttackTarget == true) {
		SetbInterToPlayer(true);
		if (AIController) {
			//AIController->StopMovement();
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
		}
		if (!bAttacking) {
			bAttacking = true;
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance) {
				AnimInstance->Montage_Play(CombatMontage, AnimPlayRate);
				AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);
			}

		}
	}
}

void AEnemy::AttackEnd()
{
	bAttacking = false;
	SetbInterToPlayer(false);
	if (bOverlappingCombatSphere) {
		float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
		GetWorldTimerManager().SetTimer(AttackTimer,this,&AEnemy::Attack, AttackTime);
		//Attack();
	}
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f) {
		Health = 0.f;
		Die(DamageCauser);
	}
	else {
		Health = Health - DamageAmount;
	}

	return DamageAmount;
}

void AEnemy::Die(AActor* Causer) 
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance) {
		AnimInstance->Montage_Play(CombatMontage, 1.f);
		AnimInstance->Montage_JumpToSection(FName("Death"), CombatMontage);
	}
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bAttacking = false;

	AMainCharacter* MC = Cast<AMainCharacter>(Causer);
	if (MC) {
		if (MC->MainCharPlayerController) {
			MC->MainCharPlayerController->HideEnemyHealthBar();
		}
		MC->UpdateCombatTarget();
	}
}

void AEnemy::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay);
}

bool AEnemy::Alive()
{
	return (GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_Dead);
}

void AEnemy::Disappear()
{
	Destroy();
}

FRotator AEnemy::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw = FRotator(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}

void AEnemy::SetbInterToPlayer(bool Interp)
{
	bInterpToPlayer = Interp;
}

float AEnemy::GetCurrentHealth() {
	return Health;
}