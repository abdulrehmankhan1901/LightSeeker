// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_PickUp UMETA(DisplayName = "Pickup"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

/**
 * 
 */
UCLASS()
class LIGHTSEEKER_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:

	AWeapon();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State")
	EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Particles")
	bool bWeaponParticles;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkeletalMesh")
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundCue* OnEquipSound;

	//inherited from Item; therefore, no need to mark these functions as UFUNCTION
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	void Equip(class AMainCharacter* Char);

	FORCEINLINE void SetWeaponState(EWeaponState NewState) { WeaponState = NewState; }
	FORCEINLINE EWeaponState GetWeaponState() { return WeaponState; }
};
