// Copyright 2026 Silvan Teufel / Teufel-Engineering.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "GameInGameWidget.generated.h"

class SGameInGamePanel;
class UMaterialInterface;

/** Broadcast when a minigame ends: GameName, final Score, and whether it set a new high score. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FGameInGameOnMinigameCompleted, const FString&, GameName, int32, Score, bool, bIsNewHighScore);

/** Broadcast when the player closes the overlay from the menu. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameInGameOnOverlayClosed);

/**
 * Designer-facing UMG wrapper around SGameInGamePanel. Drag it into any UserWidget to add
 * the drag-and-drop minigame overlay; styling is exposed as editable properties.
 */
UCLASS()
class GAMEINGAME_API UGameInGameWidget : public UWidget
{
	GENERATED_BODY()

public:
	UGameInGameWidget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Styling")
	TObjectPtr<UMaterialInterface> BorderMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Styling")
	FMargin BorderPadding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Styling")
	FLinearColor BorderTint;

	/** Corner radius in pixels applied to the border when no material is set. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Styling", meta = (ClampMin = "0.0"))
	float CornerStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool bShowTouchButtons;

	UPROPERTY(BlueprintAssignable, Category = "GameInGame|Events")
	FGameInGameOnMinigameCompleted OnMinigameCompleted;

	UPROPERTY(BlueprintAssignable, Category = "GameInGame|Events")
	FGameInGameOnOverlayClosed OnOverlayClosed;

	// UWidget interface
	virtual void SynchronizeProperties() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	void HandlePanelCompleted(const FString& GameName, int32 Score, bool bNewHighScore);
	void HandlePanelClosed();

	TSharedPtr<SGameInGamePanel> MyPanel;
};
