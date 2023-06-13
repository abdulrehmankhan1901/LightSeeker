// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickable.h"
#include "MainCharacter.h"
#include "Kismet/GameplayStatics.h" 
#include "Engine/World.h" // to get the game world

//constructor
APickable::APickable()
{
	
}

void APickable::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	//UE_LOG(LogTemp, Warning, TEXT("Pickable::OnOverlapBegin"));

	if (OtherActor) {
		AMainCharacter* MC = Cast<AMainCharacter>(OtherActor);
		if (MC) {

			BpOnPickup(MC);

			if (OverlapParticles) {
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.f), true);
			}
			if (OverlapSoundCue) {
				UGameplayStatics::PlaySound2D(this, OverlapSoundCue);
			}
			
			MC->PickupLocations.Add(GetActorLocation());

			Destroy();
		}
	}
}
void APickable::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	//UE_LOG(LogTemp, Warning, TEXT("Pickable::OnOverlapEnd"));
}