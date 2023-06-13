// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTransitionVolume.h"
#include "MainCharacter.h"

// Sets default values
ALevelTransitionVolume::ALevelTransitionVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TransitionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TransitionVolume"));
	SetRootComponent(TransitionVolume);

	//meshComponent
	meshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshComponent"));
	meshComponent->SetupAttachment(GetRootComponent());

	TransitionLevelName = "ElvenRuins";

}

// Called when the game starts or when spawned
void ALevelTransitionVolume::BeginPlay()
{
	Super::BeginPlay();

	TransitionVolume->OnComponentBeginOverlap.AddDynamic(this, &ALevelTransitionVolume::OnOverlapBegin);
	
}

// Called every frame
void ALevelTransitionVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelTransitionVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("Super::OnOverlapBegin"));

	if (OtherActor) {
		AMainCharacter* MC = Cast<AMainCharacter>(OtherActor);

		if (MC) {
			MC->SwitchLevel(TransitionLevelName);
		}
	}
}

void ALevelTransitionVolume::SetTransitionLevelName(FName LevelName)
{
	TransitionLevelName = LevelName;
}