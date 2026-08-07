// Copyright 2026 Simulated Flow All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Styling/SlateBrush.h"
#include "UObject/WeakObjectPtr.h"
#include "GameInGameTypes.h"

class SGameInGameCanvas;
class SVerticalBox;
class UGameInGameSubsystem;
class UMaterialInterface;
class IGameInGameMinigameInterface;

/** Fired when the active game ends: (GameName, FinalScore, bIsNewHighScore). */
DECLARE_DELEGATE_ThreeParams(FGameInGamePanelCompleted, const FString&, int32, bool);

/** Fired when the player requests the overlay be closed. */
DECLARE_DELEGATE(FGameInGamePanelClosed);

/**
 * The overlay panel: a material-driven bordered frame that hosts a game-selection menu
 * and an active-game viewport. Captures keyboard / gamepad focus so gameplay input never
 * leaks to the host player, and ticks the active game on real (unpausable) Slate time.
 */
class GAMEINGAME_API SGameInGamePanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SGameInGamePanel)
		: _BorderMaterial(nullptr)
		, _BorderPadding(FMargin(12.0f))
		, _BorderTint(FLinearColor::White)
		, _CornerStyle(0.0f)
		, _ShowTouchButtons(false)
	{}
		SLATE_ARGUMENT(UMaterialInterface*, BorderMaterial)
		SLATE_ARGUMENT(FMargin, BorderPadding)
		SLATE_ARGUMENT(FLinearColor, BorderTint)
		SLATE_ARGUMENT(float, CornerStyle)
		SLATE_ARGUMENT(bool, ShowTouchButtons)
		SLATE_ARGUMENT(TWeakObjectPtr<UGameInGameSubsystem>, Subsystem)
		SLATE_EVENT(FGameInGamePanelCompleted, OnMinigameCompleted)
		SLATE_EVENT(FGameInGamePanelClosed, OnOverlayClosed)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	/** Re-apply styling from the owning UMG widget without a full rebuild. */
	void UpdateStyle(UMaterialInterface* InMaterial, const FMargin& InPadding, const FLinearColor& InTint, float InCornerStyle);

	// SWidget interface
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply OnAnalogValueChanged(const FGeometry& MyGeometry, const FAnalogInputEvent& InAnalogInputEvent) override;
	virtual bool SupportsKeyboardFocus() const override { return true; }

private:
	enum class EPanelState : uint8 { Menu, Playing };

	void ApplyBrush();
	void RefreshGames();
	TSharedRef<SWidget> BuildMenu();
	TSharedRef<SWidget> BuildPlayArea();
	TSharedRef<SWidget> BuildTouchButtons();

	void StartGame(int32 Index);
	void ReturnToMenu();
	void CloseOverlay();
	void MoveSelection(int32 Delta);
	void ForwardInput(EGameInGameInput Input);
	void ForwardFlag();
	void ReportCompletion(IGameInGameMinigameInterface* Game);

	IGameInGameMinigameInterface* GetActiveGameInterface() const;
	static bool MapKey(const FKey& Key, EGameInGameInput& OutInput);

	// Styling
	UMaterialInterface* BorderMaterial = nullptr;
	FMargin BorderPadding = FMargin(12.0f);
	FLinearColor BorderTint = FLinearColor::White;
	float CornerStyle = 0.0f;
	bool bShowTouchButtons = false;
	FSlateBrush BackgroundBrush;

	// State
	EPanelState State = EPanelState::Menu;
	int32 SelectedIndex = 0;
	bool bReportedGameOver = false;

	TWeakObjectPtr<UGameInGameSubsystem> SubsystemWeak;
	TWeakObjectPtr<UObject> ActiveGame;
	TArray<TWeakObjectPtr<UObject>> Games;
	TArray<FString> GameNames;

	// Analog edge-detection state.
	bool bAnalogXActive = false;
	bool bAnalogYActive = false;

	// Callbacks
	FGameInGamePanelCompleted OnCompleted;
	FGameInGamePanelClosed OnClosed;

	// Widgets
	TSharedPtr<SGameInGameCanvas> Canvas;
	TSharedPtr<SVerticalBox> MenuList;
};
