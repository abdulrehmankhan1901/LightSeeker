// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "FloorSwitch.generated.h"

UCLASS()
class LIGHTSEEKER_API AFloorSwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloorSwitch();

	// overlap volume for triggering the functionality
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
	UBoxComponent* TriggerBox;

	// switch for the character to step on
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
	UStaticMeshComponent* FloorSwitch;

	//door to move when floor switch is stepped on
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
	UStaticMeshComponent* Door;

	//initial location for the door
	UPROPERTY(BlueprintReadWrite, Category = "Floor Switch")
	FVector InitialDoorLocation;

	//initial location for the switch
	UPROPERTY(BlueprintReadWrite, Category = "Floor Switch")
	FVector InitialFloorSwitchLocation;


	FTimerHandle FloorSwitchHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor Switch")
	float SwitchTime;


	bool bCharacterOnSwitch;

	void CloseDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch") //can implement functionality in blueprints, no need to provide in code
	void RaiseDoor();

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch") //can implement functionality in blueprints, no need to provide in code
	void LowerDoor();

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch") //can implement functionality in blueprints, no need to provide in code
	void RaiseFloorSwitch();

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch") //can implement functionality in blueprints, no need to provide in code
	void LowerFloorSwitch();

	UFUNCTION(BlueprintCallable, Category = "FloorSwitch")
	void UpdateDoorLocation(float Z);

	UFUNCTION(BlueprintCallable, Category = "FloorSwitch")
	void UpdateFloorSwitchLocation(float Z);
};
