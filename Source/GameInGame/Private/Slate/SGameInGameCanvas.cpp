// Copyright 2026 Silvan Teufel All Rights Reserved.

#include "Slate/SGameInGameCanvas.h"
#include "GameInGameMinigameInterface.h"
#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"
#include "Fonts/SlateFontInfo.h"
#include "Brushes/SlateColorBrush.h"

namespace
{
	const FSlateColorBrush& GetWhiteBrush()
	{
		static const FSlateColorBrush WhiteBrush(FLinearColor::White);
		return WhiteBrush;
	}
}

void SGameInGameCanvas::Construct(const FArguments& InArgs)
{
	SetCanTick(false); // The panel drives ticking; the canvas only paints.
}

void SGameInGameCanvas::SetActiveGame(UObject* InGameObject)
{
	ActiveGameObject = InGameObject;
}

void SGameInGameCanvas::FillRectN(float X, float Y, float W, float H, const FLinearColor& Color) const
{
	if (!CachedElementList)
	{
		return;
	}

	const FVector2D Size(W * CachedLocalSize.X, H * CachedLocalSize.Y);
	const FVector2D Offset(X * CachedLocalSize.X, Y * CachedLocalSize.Y);

	FSlateDrawElement::MakeBox(
		*CachedElementList,
		CachedLayerId,
		CachedGeometry.ToPaintGeometry(Size, FSlateLayoutTransform(Offset)),
		&GetWhiteBrush(),
		CachedDrawEffects,
		Color);
}

void SGameInGameCanvas::StrokeRectN(float X, float Y, float W, float H, const FLinearColor& Color, float ThicknessPx) const
{
	if (!CachedElementList || CachedLocalSize.X <= 0.f || CachedLocalSize.Y <= 0.f)
	{
		return;
	}

	const float TX = ThicknessPx / CachedLocalSize.X;
	const float TY = ThicknessPx / CachedLocalSize.Y;

	FillRectN(X, Y, W, TY, Color);					// Top
	FillRectN(X, Y + H - TY, W, TY, Color);			// Bottom
	FillRectN(X, Y, TX, H, Color);					// Left
	FillRectN(X + W - TX, Y, TX, H, Color);			// Right
}

void SGameInGameCanvas::DrawGridCell(int32 GX, int32 GY, int32 Cols, int32 Rows, const FLinearColor& Color, float InsetFrac) const
{
	if (Cols <= 0 || Rows <= 0)
	{
		return;
	}

	const float CW = 1.0f / Cols;
	const float CH = 1.0f / Rows;
	const float IX = CW * InsetFrac;
	const float IY = CH * InsetFrac;

	FillRectN(GX * CW + IX, GY * CH + IY, CW - 2.0f * IX, CH - 2.0f * IY, Color);
}

void SGameInGameCanvas::DrawTextN(const FString& Text, float X, float Y, const FLinearColor& Color, int32 FontSize) const
{
	if (!CachedElementList)
	{
		return;
	}

	const FVector2D Offset(X * CachedLocalSize.X, Y * CachedLocalSize.Y);
	const FSlateFontInfo Font = FCoreStyle::GetDefaultFontStyle("Regular", FontSize);

	FSlateDrawElement::MakeText(
		*CachedElementList,
		CachedLayerId + 1,
		CachedGeometry.ToPaintGeometry(CachedLocalSize, FSlateLayoutTransform(Offset)),
		Text,
		Font,
		CachedDrawEffects,
		Color);
}

int32 SGameInGameCanvas::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	CachedElementList = &OutDrawElements;
	CachedGeometry = AllottedGeometry;
	CachedLayerId = LayerId;
	CachedLocalSize = FVector2D(AllottedGeometry.GetLocalSize());
	CachedDrawEffects = bParentEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

	if (UObject* Obj = ActiveGameObject.Get())
	{
		if (IGameInGameMinigameInterface* Game = Cast<IGameInGameMinigameInterface>(Obj))
		{
			Game->Render(const_cast<SGameInGameCanvas*>(this));
		}
	}

	CachedElementList = nullptr;
	return LayerId + 10;
}

FVector2D SGameInGameCanvas::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	return FVector2D(320.0f, 320.0f);
}
