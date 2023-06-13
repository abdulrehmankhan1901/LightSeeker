// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "MainCharacter.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"


AWeapon::AWeapon()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetRootComponent());

	

	bWeaponParticles = false;

	WeaponState = EWeaponState::EWS_PickUp;

	Damage = 25.f;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapEnd);

	// setup collision for only when overlapping with enemy
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if ((WeaponState == EWeaponState::EWS_PickUp) && OtherActor) {
		AMainCharacter* MC = Cast<AMainCharacter>(OtherActor);
		if (MC) {
			//Equip(MC);
			MC->SetActiveOverlappingItem(this);
		}
	}
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	if (OtherActor) {
		AMainCharacter* MC = Cast<AMainCharacter>(OtherActor);
		if (MC) {
			MC->SetActiveOverlappingItem(nullptr);
		}
	}
}

void AWeapon::Equip(AMainCharacter* Char)
{
	if (Char) {
		// set instigator for damagin the enemy
		SetInstigator(Char->GetController());
		// prevent camera from zooming in when the weapon and player are in line with eachother
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore); 
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		// ignore physics for the weapon when equipping
		SkeletalMesh->SetSimulatePhysics(false);

		const USkeletalMeshSocket* RightHandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket");
		if (RightHandSocket) {
			RightHandSocket->AttachActor(this, Char->GetMesh());

			bRotate = false;

			
			Char->SetEquippedWeapon(this);
			Char->SetActiveOverlappingItem(nullptr);

			if (OnEquipSound) {
				UGameplayStatics::PlaySound2D(this, OnEquipSound);
			}

			if (!bWeaponParticles) {
				IdleParticlesComponent->Deactivate();
			}
		}
	}
}

void AWeapon::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor) {
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy) {
			if (Enemy->HitParticles) {
				const USkeletalMeshSocket* WeaponSocket = SkeletalMesh->GetSocketByName("WeaponSocket");
				if (WeaponSocket) {
					FVector SocketLocation = WeaponSocket->GetSocketLocation(SkeletalMesh);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, SocketLocation, FRotator(0.f), false);
				}
			}
			if (Enemy->HitSound) {
				UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
			}
			if (DamageTypeClass) {
				UGameplayStatics::ApplyDamage(Enemy, Damage, WeaponInstigator, this, DamageTypeClass);
			}
		}
	}
}

void AWeapon::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}


void AWeapon::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

}

void AWeapon::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}