// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Critter.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));

}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	
	if (ActorToSpawn1) {
		SpawnArray.Add(ActorToSpawn1);
	}
	if (ActorToSpawn2) {
		SpawnArray.Add(ActorToSpawn2);
	}
	if (ActorToSpawn3) {
		SpawnArray.Add(ActorToSpawn3);
	}
	if (ActorToSpawn4) {
		SpawnArray.Add(ActorToSpawn1);
	}
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ASpawnVolume::GetSpawnPoint()
{
	FVector Extent = SpawningBox->GetScaledBoxExtent();
	FVector Origin = SpawningBox->GetComponentLocation();

	FVector Point = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);

	return Point;
}

void ASpawnVolume::SpawnActor_Implementation(UClass* ToSpawn, const FVector& Location)	//mandatory when coding BlueprintNativeEvents, _Implementation tells UE that this is the implementation
{
	if (ToSpawn) {
		UWorld* World = GetWorld();
		FActorSpawnParameters SpawnParameters;

		if (World) {
			World->SpawnActor<AActor>(ToSpawn, Location, FRotator(0.f), SpawnParameters); //creates and returns the specified spawn
		}
	}
}

TSubclassOf<AActor> ASpawnVolume::GetSpawnActor() 
{
	if (SpawnArray.Num() > 0) {
		int32 Selection = FMath::RandRange(0, SpawnArray.Num() - 1);

		return SpawnArray[Selection];
	}
	else {
		return nullptr;
	}
}