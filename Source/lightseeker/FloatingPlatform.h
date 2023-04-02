// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingPlatform.generated.h"

UCLASS()
class LIGHTSEEKER_API AFloatingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingPlatform();

	/** Mesh for the floating platform	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floating Platform")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, Category = "Floating Platform")
	FVector StartPoint;

	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"), Category = "Floating Platform")
	FVector EndPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Platform")
	float InterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Platform")
	float InterpTime;


	FTimerHandle InterpTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Platform")
	bool bInterping;


	float Distance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ToggleInterping(); //only for toggling the bInterping variable

	void SwapVectors(FVector& Start, FVector& End); //swap the values of the passed vectors (startpoint and endpoint)

};
