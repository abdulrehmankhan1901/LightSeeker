// Fill out your copyright notice in the Description page of Project Settings.


#include "Collider.h"
#include "ColliderMovementComponent.h"

// Sets default values
ACollider::ACollider()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	//rootComponent
	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	//sphereComponent
	sphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("sphereComponent"));
	//sphereComponent->SetupAttachment(GetRootComponent());
	SetRootComponent(sphereComponent); //set as root for movement
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
		meshComponent->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
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

	//colliderMovementComponent
	colliderMovementComponent = CreateDefaultSubobject<UColliderMovementComponent>(TEXT("colliderMovementComponent"));
	colliderMovementComponent->UpdatedComponent = RootComponent;

	//cameraInput
	cameraInput = FVector2D(0.f,0.f);

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

	//for Camera
	FRotator newRotation = GetActorRotation();
	newRotation.Yaw += cameraInput.X;
	SetActorRotation(newRotation);

	//for SpringArm
	FRotator newSpringArmRotation = springArm->GetComponentRotation();
	newSpringArmRotation.Pitch = FMath::Clamp(newSpringArmRotation.Pitch += cameraInput.Y, -80.f, -15.f); //never get below or up the player
	springArm->SetWorldRotation(newSpringArmRotation);
}

// Called to bind functionality to input
void ACollider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACollider::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACollider::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("CameraPitch"), this, &ACollider::PitchCamera);
	PlayerInputComponent->BindAxis(TEXT("CameraYaw"), this, &ACollider::YawnCamera);

}

void ACollider::MoveForward(float value) 
{
	FVector forward = GetActorForwardVector();
	//AddMovementInput(value * forward);
	if (colliderMovementComponent) {
		colliderMovementComponent->AddInputVector(forward * value);
	}
}
void ACollider::MoveRight(float value) 
{
	FVector right = GetActorRightVector();
	//AddMovementInput(value * right);
	if (colliderMovementComponent) {
		colliderMovementComponent->AddInputVector(right * value);
	}
}
void ACollider::YawnCamera(float axisValue) {	//Yaw*
	cameraInput.X = axisValue;
}
void ACollider::PitchCamera(float axisValue) {
	cameraInput.Y = axisValue;
}
UPawnMovementComponent* ACollider::GetMovementComponent() const{
	return colliderMovementComponent;
}