// Copyright 2026 Silvan Teufel All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SLeafWidget.h"
#include "UObject/WeakObjectPtr.h"

/**
 * Lightweight leaf widget the active minigame draws into. It caches the paint context
 * during OnPaint and exposes normalized [0..1] draw helpers so games render in a
 * resolution-independent space without touching FSlateDrawElement directly.
 */
class GAMEINGAME_API SGameInGameCanvas : public SLeafWidget
{
public:
	SLATE_BEGIN_ARGS(SGameInGameCanvas) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	/** UObject that implements IGameInGameMinigameInterface, or nullptr to clear. */
	void SetActiveGame(UObject* InGameObject);

	// --- Draw helpers (valid only while called from a game's Render). Coordinates are normalized. ---

	/** Fill an axis-aligned rectangle. */
	void FillRectN(float X, float Y, float W, float H, const FLinearColor& Color) const;

	/** Stroke an axis-aligned rectangle outline (built from thin filled bars). */
	void StrokeRectN(float X, float Y, float W, float H, const FLinearColor& Color, float ThicknessPx = 1.0f) const;

	/** Draw a single cell of a Cols x Rows grid with a fractional inset (0..0.5). */
	void DrawGridCell(int32 GX, int32 GY, int32 Cols, int32 Rows, const FLinearColor& Color, float InsetFrac = 0.06f) const;

	/** Draw a text string anchored at the normalized top-left position. */
	void DrawTextN(const FString& Text, float X, float Y, const FLinearColor& Color, int32 FontSize = 12) const;

	/** Canvas size in local (pre-DPI) Slate units. Valid during Render. */
	FVector2D GetCanvasLocalSize() const { return CachedLocalSize; }

	// --- SWidget interface ---
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;

private:
	TWeakObjectPtr<UObject> ActiveGameObject;

	// Paint context cached for the duration of a single OnPaint pass (mutable: OnPaint is const).
	mutable FSlateWindowElementList* CachedElementList = nullptr;
	mutable FGeometry CachedGeometry;
	mutable int32 CachedLayerId = 0;
	mutable FVector2D CachedLocalSize = FVector2D::ZeroVector;
	mutable ESlateDrawEffect CachedDrawEffects = ESlateDrawEffect::None;
};
