// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosive.h"
#include "MainCharacter.h"
#include "Kismet/GameplayStatics.h" 
#include "Engine/World.h" // to get the game world
#include "Enemy.h"


//constructor
AExplosive::AExplosive()
{
	Damage = 15.f; // default value for explosion damage
}

void AExplosive::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	//UE_LOG(LogTemp, Warning, TEXT("Explosive::OnOverlapBegin"));

	if (OtherActor) {
		AMainCharacter* MC = Cast<AMainCharacter>(OtherActor); // will return NULL if casting not possible/OtherActor is NULL
		//AEnemy* Enemy = Cast<AEnemy>(OtherActor); // allow the player to run the enemy into the explosion to give it damage as well
		if (MC) {
			if (OverlapParticles) {
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.f), true);
			}
			if (OverlapSoundCue) {
				UGameplayStatics::PlaySound2D(this, OverlapSoundCue);
			}
			//MC->DecreaseHealth(Damage);
			UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, this, DamageTypeClass);
			Destroy();
		}
	}
}
void AExplosive::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	//UE_LOG(LogTemp, Warning, TEXT("Explosive::OnOverlapEnd"));
}