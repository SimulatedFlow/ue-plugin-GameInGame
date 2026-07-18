// Copyright 2026 Silvan Teufel / Teufel-Engineering.com All Rights Reserved.

#include "GameInGameWidget.h"
#include "Slate/SGameInGamePanel.h"
#include "GameInGameSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "GameInGame"

UGameInGameWidget::UGameInGameWidget()
	: BorderMaterial(nullptr)
	, BorderPadding(FMargin(12.0f))
	, BorderTint(FLinearColor::White)
	, CornerStyle(0.0f)
	, bShowTouchButtons(false)
{
}

TSharedRef<SWidget> UGameInGameWidget::RebuildWidget()
{
	TWeakObjectPtr<UGameInGameSubsystem> Subsystem;
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			Subsystem = GameInstance->GetSubsystem<UGameInGameSubsystem>();
		}
	}

	MyPanel = SNew(SGameInGamePanel)
		.BorderMaterial(BorderMaterial)
		.BorderPadding(BorderPadding)
		.BorderTint(BorderTint)
		.CornerStyle(CornerStyle)
		.ShowTouchButtons(bShowTouchButtons)
		.Subsystem(Subsystem)
		.OnMinigameCompleted(FGameInGamePanelCompleted::CreateUObject(this, &UGameInGameWidget::HandlePanelCompleted))
		.OnOverlayClosed(FGameInGamePanelClosed::CreateUObject(this, &UGameInGameWidget::HandlePanelClosed));

	return MyPanel.ToSharedRef();
}

void UGameInGameWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (MyPanel.IsValid())
	{
		MyPanel->UpdateStyle(BorderMaterial, BorderPadding, BorderTint, CornerStyle);
	}
}

void UGameInGameWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyPanel.Reset();
}

void UGameInGameWidget::HandlePanelCompleted(const FString& GameName, int32 Score, bool bNewHighScore)
{
	OnMinigameCompleted.Broadcast(GameName, Score, bNewHighScore);
}

void UGameInGameWidget::HandlePanelClosed()
{
	OnOverlayClosed.Broadcast();
}

#if WITH_EDITOR
const FText UGameInGameWidget::GetPaletteCategory()
{
	return LOCTEXT("PaletteCategory", "Game In Game");
}
#endif

#undef LOCTEXT_NAMESPACE
