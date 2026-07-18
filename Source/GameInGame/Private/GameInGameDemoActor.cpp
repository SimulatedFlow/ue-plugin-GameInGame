// Copyright 2026 Silvan Teufel / Teufel-Engineering.com All Rights Reserved.

#include "GameInGameDemoActor.h"
#include "Slate/SGameInGamePanel.h"
#include "GameInGameSubsystem.h"

#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "Widgets/Layout/SBox.h"
#include "Framework/Application/SlateApplication.h"

AGameInGameDemoActor::AGameInGameDemoActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGameInGameDemoActor::BeginPlay()
{
	Super::BeginPlay();

	if (bShowOnBeginPlay)
	{
		ShowOverlay();
	}
}

void AGameInGameDemoActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	HideOverlay();
	Super::EndPlay(EndPlayReason);
}

void AGameInGameDemoActor::ShowOverlay()
{
	UWorld* World = GetWorld();
	if (!World || !World->GetGameViewport())
	{
		return;
	}

	if (OverlayPanel.IsValid())
	{
		return; // Already shown.
	}

	TWeakObjectPtr<UGameInGameSubsystem> Subsystem;
	if (UGameInstance* GameInstance = World->GetGameInstance())
	{
		Subsystem = GameInstance->GetSubsystem<UGameInGameSubsystem>();
	}

	OverlayPanel = SNew(SGameInGamePanel)
		.BorderMaterial(BorderMaterial)
		.BorderPadding(FMargin(16.0f))
		.BorderTint(BorderTint)
		.CornerStyle(CornerStyle)
		.ShowTouchButtons(bShowTouchButtons)
		.Subsystem(Subsystem)
		.OnOverlayClosed(FGameInGamePanelClosed::CreateUObject(this, &AGameInGameDemoActor::HandleOverlayClosed));

	// Center a fixed-size frame on screen.
	ViewportContent =
		SNew(SBox)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.WidthOverride(420.0f)
			.HeightOverride(560.0f)
			[
				OverlayPanel.ToSharedRef()
			]
		];

	World->GetGameViewport()->AddViewportWidgetContent(ViewportContent.ToSharedRef(), 100);
	FSlateApplication::Get().SetKeyboardFocus(OverlayPanel);
}

void AGameInGameDemoActor::HideOverlay()
{
	if (UWorld* World = GetWorld())
	{
		if (UGameViewportClient* Viewport = World->GetGameViewport())
		{
			if (ViewportContent.IsValid())
			{
				Viewport->RemoveViewportWidgetContent(ViewportContent.ToSharedRef());
			}
		}
	}
	ViewportContent.Reset();
	OverlayPanel.Reset();
}

void AGameInGameDemoActor::HandleOverlayClosed()
{
	HideOverlay();
}
