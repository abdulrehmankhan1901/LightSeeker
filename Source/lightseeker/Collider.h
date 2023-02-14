// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerInput.h"
#include "Collider.generated.h"

UCLASS()
class LIGHTSEEKER_API ACollider : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACollider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	UStaticMeshComponent* meshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	USphereComponent* sphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	UCameraComponent* camera;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	USpringArmComponent* springArm;

	//movementComponent
	UPROPERTY(VisibleAnywhere, Category="Movement")
	class UColliderMovementComponent* colliderMovementComponent;

	virtual UPawnMovementComponent* GetMovementComponent() const override;

	//-----GETTERS & SETTERS
	//forceinline: allows to see the function after it as a macro. more efficient, no need to jump to definition everytime it is called. better to use with getters and setters
	//meshComponent
	FORCEINLINE UStaticMeshComponent* getMeshComponent() { return meshComponent; }
	FORCEINLINE void setMeshComponent(UStaticMeshComponent* newMeshComponent) { meshComponent = newMeshComponent; }
	//sphereComponent
	FORCEINLINE USphereComponent* getSphereComponent() { return sphereComponent; }
	FORCEINLINE void setSphereComponent(USphereComponent* newSphereComponent) { sphereComponent = newSphereComponent; }
	//camera
	FORCEINLINE UCameraComponent* getCameraComponent() { return camera; }
	FORCEINLINE	void setCamerComponent(UCameraComponent* newCameraComponent) { camera = newCameraComponent; }
	//springArm
	FORCEINLINE USpringArmComponent* getSpringArmComponent() { return springArm; }
	FORCEINLINE void setSpringArmComponent(USpringArmComponent* newSpringArmComponent) { springArm = newSpringArmComponent; }

private:

	void MoveForward(float value);
	void MoveRight(float value);
	void YawnCamera(float axisValue);
	void PitchCamera(float axisValue);

	FVector2D cameraInput;
};
