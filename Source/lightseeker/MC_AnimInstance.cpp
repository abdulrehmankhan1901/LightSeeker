// Fill out your copyright notice in the Description page of Project Settings.


#include "MC_AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MainCharacter.h"

void UMC_AnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr) {
		Pawn = TryGetPawnOwner();
		if (Pawn) {
			MainCharacter = Cast<AMainCharacter>(Pawn);
		}
	}

}

void UMC_AnimInstance::UpdateAnimProps()
{
	if (Pawn == nullptr) {
		Pawn = TryGetPawnOwner();
	}

	if (Pawn) {
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f); // for in air
		MovementSpeed = LateralSpeed.Size();

		bInAir = Pawn->GetMovementComponent()->IsFalling();

		if (MainCharacter == nullptr) {
			MainCharacter = Cast<AMainCharacter>(Pawn);
		}
	}
}