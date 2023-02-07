// Fill out your copyright notice in the Description page of Project Settings.


#include "Collider.h"

// Sets default values
ACollider::ACollider()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//rootComponent
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	//sphereComponent
	sphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("sphereComponent"));
	sphereComponent->SetupAttachment(GetRootComponent());
	sphereComponent->InitSphereRadius(40.f);
	//--sphereComponent->SetRelativeLocation(FVector(0.f));
	sphereComponent->SetCollisionProfileName(TEXT("Pawn"));

	//meshComponent
	meshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshComponent"));
	meshComponent->SetupAttachment(GetRootComponent());
	// hardcode mesh asset [NOT RECOMMENDED]
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshComponentAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'")); //creats the mesh component asset
	if (meshComponentAsset.Succeeded()) {
		meshComponent->SetStaticMesh(meshComponentAsset.Object); // static mesh from this asset gets stored in the 'Object'
		meshComponent->SetRelativeLocation(FVector(0.f, 0.f, -40.f));
		meshComponent->SetWorldScale3D(FVector(0.8f, 0.8f, 0.8f));
	}

	//springArm
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("springArm"));
	springArm->SetupAttachment(GetRootComponent());
	springArm->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
	springArm->TargetArmLength = 400.f;
	springArm->bEnableCameraLag = true;
	springArm->CameraLagSpeed = 3.f;

	//camera
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("camera"));
	camera->SetupAttachment(springArm, USpringArmComponent::SocketName);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ACollider::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACollider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACollider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACollider::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACollider::MoveRight);

}

void ACollider::MoveForward(float value) 
{
	FVector forward = GetActorForwardVector();
	AddMovementInput(value * forward);
}
void ACollider::MoveRight(float value) 
{
	FVector right = GetActorRightVector();
	AddMovementInput(value * right);
}