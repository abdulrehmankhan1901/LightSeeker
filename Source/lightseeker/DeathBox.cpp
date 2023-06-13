// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathBox.h"
#include "MainCharacter.h"

// Sets default values
ADeathBox::ADeathBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DeathVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("DeathVolume"));
	SetRootComponent(DeathVolume);

}

// Called when the game starts or when spawned
void ADeathBox::BeginPlay()
{
	Super::BeginPlay();

	DeathVolume->OnComponentBeginOverlap.AddDynamic(this, &ADeathBox::OnOverlapBegin);
	
}

// Called every frame
void ADeathBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADeathBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("Super::OnOverlapBegin"));

	if (OtherActor) {
		AMainCharacter* MC = Cast<AMainCharacter>(OtherActor);

		if (MC) {
			MC->Die();
		}
	}
}