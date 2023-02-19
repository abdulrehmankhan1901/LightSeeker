// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MC_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class LIGHTSEEKER_API UMC_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeInitializeAnimation() override;

	// to update the character variables
	UFUNCTION(BlueprintCallable, Category = "AnimationProperty")
	void UpdateAnimProps();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MovementSpeed; //speed of the character: Apwan* Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bInAir; //for checking if in air

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	class APawn* Pawn; //for character

	

};
