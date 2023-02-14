// Fill out your copyright notice in the Description page of Project Settings.


#include "Critter.h"

// Sets default values - Constructor
ACritter::ACritter()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("rootComponent")); //create root component; RootComponent is a default variable for initialising the root component
	meshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("meshComponent")); //create mesh component
	meshComponent->SetupAttachment(GetRootComponent()); //attach mesh component to the root component

	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("camera")); //create camera component
	camera->SetupAttachment(GetRootComponent());
	camera->SetRelativeLocation(FVector(-300.f, 0.f, 300.f));
	camera->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));

	//AutoPossessPlayer = EAutoReceiveInput::Player0; //player 0 refers to the first and only player when used in singleplayer games and the first player in multiplayer games

	//player variables
	currentVelocity = FVector(0.f);
	maxSpeed = 100.f;
}

// Called when the game starts or when spawned
void ACritter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ACritter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//alter position with player Input
	FVector newLocation = GetActorLocation() + (currentVelocity * DeltaTime);
	SetActorLocation(newLocation);
}

// Called to bind functionality to input
void ACritter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACritter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACritter::MoveRight);
}

//Critter.h functions
//X and Y are according to the placement of the camera. If you want to change currentVelocity.Y with MoveForward, make sure the camera is behind the player.
void ACritter::MoveForward(float value)
{
	currentVelocity.X = FMath::Clamp(value, -1.f, 1.f) * maxSpeed; //clamp: takes a min and max and ensures will always be: min<=value<=max;
}
void ACritter::MoveRight(float value)
{
	currentVelocity.Y = FMath::Clamp(value, -1.f, 1.f) * maxSpeed; //clamp: takes a min and max and ensures will always be: min<=value<=max;
}