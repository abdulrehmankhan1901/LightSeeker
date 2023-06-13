// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enemy.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class LIGHTSEEKER_API UEnemyAnimInstance : public UAnimInstance
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
	class APawn* Pawn; //for character

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	class AEnemy* Enemy;
};
