// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "SpawnVolume.generated.h"

UCLASS()
class LIGHTSEEKER_API ASpawnVolume : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASpawnVolume();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	UBoxComponent* SpawningBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<class AActor> ActorToSpawn1; //specifies a class that can be chosen from a drop down inside the editor

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<class AActor> ActorToSpawn2; //specifies a class that can be chosen from a drop down inside the editor

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<class AActor> ActorToSpawn3; //specifies a class that can be chosen from a drop down inside the editor

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<class AActor> ActorToSpawn4; //specifies a class that can be chosen from a drop down inside the editor

	// for storing the actors to spawn
	TArray<TSubclassOf<AActor>> SpawnArray;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "Spawning") //BlueprintPure: only returns a value, has no input connections in blueprints
	FVector GetSpawnPoint();

	UFUNCTION(BlueprintPure, Category = "Spawning") //BlueprintPure: only returns a value, has no input connections in blueprints
	TSubclassOf<AActor> GetSpawnActor();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Spawning")	//BlueprintNativeEvent: have functionality for both C++ and Blueprints, a hybrid;
	void SpawnActor(UClass* ToSpawn, const FVector& Location);	//UClass: highest in hierarchy, allows to be a general pass; 

};
