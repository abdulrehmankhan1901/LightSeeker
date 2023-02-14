// Fill out your copyright notice in the Description page of Project Settings.


#include "ColliderMovementComponent.h"

void UColliderMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime)) {
		return;
	}

	FVector desiredMovementThisFrame = ConsumeInputVector().GetClampedToMaxSize(1.0f); //reset the parameters after returning the value

	if (!desiredMovementThisFrame.IsNearlyZero()) {
		FHitResult hit;
		SafeMoveUpdatedComponent(desiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, hit);

		//if we bump into something, slide along the side of it
		if (hit.IsValidBlockingHit()) {
			SlideAlongSurface(desiredMovementThisFrame, 1.f - hit.Time, hit.Normal, hit);
			UE_LOG(LogTemp, Warning, TEXT("Valid Hit Blocking"));
		}
	}
}