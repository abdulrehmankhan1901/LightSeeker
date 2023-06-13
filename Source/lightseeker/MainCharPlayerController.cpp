// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharPlayerController.h"

void AMainCharPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDOverlayAsset) {
		
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);

	}
	HUDOverlay->AddToViewport();
	HUDOverlay->SetVisibility(ESlateVisibility::Visible);

	if (WEnemyHealthBar) {
		EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);
		if (EnemyHealthBar) {
			EnemyHealthBar->AddToViewport();
			EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
		}
		FVector2D Vec = FVector2d(0.f,0.f); // alignment vector
		EnemyHealthBar->SetAlignmentInViewport(Vec);
	}

	if (WPauseMenu) {
		PauseMenu = CreateWidget<UUserWidget>(this, WPauseMenu);
		if (PauseMenu) {
			PauseMenu->AddToViewport();
			PauseMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AMainCharPlayerController::DisplayEnemyHealthBar()
{
	if (EnemyHealthBar) {
		bEnemyHealthBarVisible = true;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainCharPlayerController::HideEnemyHealthBar()
{
	if (EnemyHealthBar) {
		bEnemyHealthBarVisible = false;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainCharPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyHealthBar) {
		FVector2D PositionInViewport; // the coords (0,0) are the topleft of the screen, goind down increases the y-axis and going left increases the x-axis
		ProjectWorldLocationToScreen(EnemyLocation, PositionInViewport);
		PositionInViewport = PositionInViewport - 80.f;

		FVector2D SizeInViewport = FVector2D(300.f, 25.f);

		EnemyHealthBar->SetPositionInViewport(PositionInViewport);
		EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
	}
}

void AMainCharPlayerController::DisplayPauseMenu_Implementation()
{
	if (PauseMenu) {
		bPauseMenuVisible = true;
		PauseMenu->SetVisibility(ESlateVisibility::Visible);

		FInputModeGameAndUI InputMode;

		SetInputMode(InputMode);
		bShowMouseCursor = true;
	}
}

void AMainCharPlayerController::HidePauseMenu_Implementation()
{
	if (PauseMenu) {
		bPauseMenuVisible = false;

		FInputModeGameOnly InputMode;

		SetInputMode(InputMode);
		bShowMouseCursor = false;
	}
}

void AMainCharPlayerController::TogglePauseMenu()
{
	if (bPauseMenuVisible) {
		HidePauseMenu();
	}
	else {
		DisplayPauseMenu();
	}
}