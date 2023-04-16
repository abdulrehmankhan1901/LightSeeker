// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "MainCharPlayerController.generated.h"

/**
 * Add UMG in build.cs->PublicDependencyModuleNames for HUD. UMG: Unreal Motion Grpahics; Heads Up Display;
 * Add "Slate", "SlateCore" in build.cs->PrivateDependencyModuleNames for HUD
 */
UCLASS()
class LIGHTSEEKER_API AMainCharPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	// reference to the UMG asset in the editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> HUDOverlayAsset;

	// variable to hold the widget after it is created
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* HUDOverlay;

protected:

	virtual void BeginPlay() override;
};
