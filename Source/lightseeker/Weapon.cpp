// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "MainCharacter.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"


AWeapon::AWeapon()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	bWeaponParticles = false;

	WeaponState = EWeaponState::EWS_PickUp;
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