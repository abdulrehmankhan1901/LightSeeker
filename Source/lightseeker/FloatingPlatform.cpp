// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingPlatform.h"
#include "TimerManager.h"

// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = StaticMesh;

	StartPoint = FVector(0.f);
	EndPoint = FVector(0.f);

	InterpSpeed = 3.0f;
	InterpTime = 1.f;

	bInterping = false;

}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();
	StartPoint = GetActorLocation();
	EndPoint = EndPoint + StartPoint;


	GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, InterpTime);

	Distance = (EndPoint - StartPoint).Size(); //.Size: because both points are vectors and we need the distance between these vectors
	
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterping) {
		FVector CurrentLocation = GetActorLocation();
		/** Platform Moving Logic	*/
		FVector Interpolate = FMath::VInterpTo(CurrentLocation, EndPoint, DeltaTime, InterpSpeed);
		SetActorLocation(Interpolate);

		float DistanceTraveled = (GetActorLocation() - StartPoint).Size(); // how far we have traveled from the beginning

		/** Return the platform to its original position */
		if (Distance - DistanceTraveled <= 1.f) {
			ToggleInterping();
			GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, InterpTime);
			SwapVectors(StartPoint, EndPoint);
		}
	}

}

void AFloatingPlatform::ToggleInterping()
{
	bInterping = !bInterping;
}

void AFloatingPlatform::SwapVectors(FVector& Start, FVector& End)
{
	FVector Temp = Start;
	Start = End;
	End = Temp;
}